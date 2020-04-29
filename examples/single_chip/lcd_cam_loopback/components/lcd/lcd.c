
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "esp32s3beta/rom/lldesc.h"
#include "soc/system_reg.h"
#include "esp_log.h"
#include "lcd.h"

static const char *TAG = "lcd";

#define LCD_DMA_MAX_SIZE     (4095)

typedef struct {
    uint32_t buffer_size;
    uint32_t half_buffer_size;
    uint32_t node_cnt;
    uint32_t half_node_cnt;
    uint32_t dma_size;
    uint8_t horizontal;
    uint8_t dc_state;
    uint8_t pin_dc;
    uint8_t pin_cs;
    uint8_t pin_rst;
    uint8_t pin_bk;
    lldesc_t *dma;
    uint8_t *buffer;
    QueueHandle_t event_queue;
} lcd_obj_t;

static lcd_obj_t *lcd_obj = NULL;

void inline lcd_set_rst(uint8_t state)
{
    gpio_set_level(lcd_obj->pin_rst, state);
}

void inline lcd_set_dc(uint8_t state)
{
    gpio_set_level(lcd_obj->pin_dc, state);
}

void inline lcd_set_cs(uint8_t state)
{
    gpio_set_level(lcd_obj->pin_cs, state);
}

void inline lcd_set_blk(uint8_t state)
{
    gpio_set_level(lcd_obj->pin_bk, state);
}

static void IRAM_ATTR lcd_isr(void *arg)
{
    int event;
    BaseType_t HPTaskAwoken = pdFALSE;
    typeof(GPSPI3.dma_int_st) int_st = GPSPI3.dma_int_st;
    GPSPI3.dma_int_clr.val = int_st.val;
    // ets_printf("intr: 0x%x\n", int_st);

    if (int_st.out_eof) {
        xQueueSendFromISR(lcd_obj->event_queue, &int_st.val, &HPTaskAwoken);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void spi_write_data(uint8_t *data, size_t len)
{
    int event  = 0;
    int x = 0, cnt = 0, size = 0;
    int end_pos = 0;
    lcd_set_dc(lcd_obj->dc_state);
    // 生成一段数据DMA链表
    for (x = 0; x < lcd_obj->node_cnt; x++) {
        lcd_obj->dma[x].size = lcd_obj->dma_size;
        lcd_obj->dma[x].length = lcd_obj->dma_size;
        lcd_obj->dma[x].buf = (lcd_obj->buffer + lcd_obj->dma_size * x);
        lcd_obj->dma[x].eof = !((x + 1) % lcd_obj->half_node_cnt);
        lcd_obj->dma[x].empty = &lcd_obj->dma[(x + 1) % lcd_obj->node_cnt];
    }
    lcd_obj->dma[lcd_obj->half_node_cnt - 1].empty = NULL;
    lcd_obj->dma[lcd_obj->node_cnt - 1].empty = NULL;
    cnt = len / lcd_obj->half_buffer_size;
    // 启动信号
    xQueueSend(lcd_obj->event_queue, &event, 0);
    // 处理完整一段数据， 乒乓操作
    for (x = 0; x < cnt; x++) {
        memcpy(lcd_obj->dma[(x % 2) * lcd_obj->half_node_cnt].buf, data, lcd_obj->half_buffer_size);
        data += lcd_obj->half_buffer_size;
        xQueueReceive(lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        GPSPI3.mosi_dlen.usr_mosi_bit_len = lcd_obj->half_buffer_size * 8 - 1;
        GPSPI3.dma_out_link.addr = ((uint32_t)&lcd_obj->dma[(x % 2) * lcd_obj->half_node_cnt]) & 0xfffff;
        GPSPI3.dma_out_link.start = 1;
        ets_delay_us(1);
        GPSPI3.cmd.usr = 1;
    }
    cnt = len % lcd_obj->half_buffer_size;
    // 处理剩余非完整段数据
    if (cnt) {
        memcpy(lcd_obj->dma[(x % 2) * lcd_obj->half_node_cnt].buf, data, cnt);
        // 处理数据长度为 lcd_obj->dma_size 的整数倍情况
        if (cnt % lcd_obj->dma_size) {
            end_pos = (x % 2) * lcd_obj->half_node_cnt + cnt / lcd_obj->dma_size;
            size = cnt % lcd_obj->dma_size;
        } else {
            end_pos = (x % 2) * lcd_obj->half_node_cnt + cnt / lcd_obj->dma_size - 1;
            size = lcd_obj->dma_size;
        }
        // 处理尾节点，使其成为 DMA 尾
        lcd_obj->dma[end_pos].size = size;
        lcd_obj->dma[end_pos].length = size;
        lcd_obj->dma[end_pos].eof = 1;
        lcd_obj->dma[end_pos].empty = NULL;
        xQueueReceive(lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
        GPSPI3.mosi_dlen.usr_mosi_bit_len = cnt * 8 - 1;
        GPSPI3.dma_out_link.addr = ((uint32_t)&lcd_obj->dma[(x % 2) * lcd_obj->half_node_cnt]) & 0xfffff;
        GPSPI3.dma_out_link.start = 1;
        ets_delay_us(1);
        GPSPI3.cmd.usr = 1;
    }
    xQueueReceive(lcd_obj->event_queue, (void *)&event, portMAX_DELAY);
}

static void lcd_delay_ms(uint32_t time)
{
    vTaskDelay(time / portTICK_RATE_MS);
}

static void lcd_write_cmd(uint8_t data)
{
    lcd_obj->dc_state = 0;
    spi_write_data(&data, 1);
}

static void lcd_write_byte(uint8_t data)
{
    lcd_obj->dc_state = 1;
    spi_write_data(&data, 1);
}

void lcd_write_data(uint8_t *data, size_t len)
{
    if (len <= 0) {
        return;
    }
    lcd_obj->dc_state = 1;
    spi_write_data(data, len);
}

void lcd_rst()
{
    lcd_set_rst(0);
    lcd_delay_ms(100);
    lcd_set_rst(1);
    lcd_delay_ms(100);
}

static void lcd_st7789_config(lcd_config_t *config)
{
    lcd_set_cs(0);

    lcd_write_cmd(0x36); // MADCTL (36h): Memory Data Access Control

    switch (config->horizontal) {
        case 0: {
            lcd_write_byte(0x00);
        }
        break;

        case 1: {
            lcd_write_byte(0xC0);
        }
        break;

        case 2: {
            lcd_write_byte(0x70);
        }
        break;

        case 3: {
            lcd_write_byte(0xA0);
        }
        break;

        default: {
            lcd_write_byte(0x00);
        }
        break;
    }

    lcd_write_cmd(0x3A);  // COLMOD (3Ah): Interface Pixel Format 
    lcd_write_byte(0x05);

    lcd_write_cmd(0xB2); // PORCTRL (B2h): Porch Setting 
    lcd_write_byte(0x0C);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x00);
    lcd_write_byte(0x33);
    lcd_write_byte(0x33); 

    lcd_write_cmd(0xB7); // GCTRL (B7h): Gate Control 
    lcd_write_byte(0x35);  

    lcd_write_cmd(0xBB); // VCOMS (BBh): VCOM Setting 
    lcd_write_byte(0x19);

    lcd_write_cmd(0xC0); // LCMCTRL (C0h): LCM Control 
    lcd_write_byte(0x2C);

    lcd_write_cmd(0xC2); // VDVVRHEN (C2h): VDV and VRH Command Enable
    lcd_write_byte(0x01);

    lcd_write_cmd(0xC3); // VRHS (C3h): VRH Set
    lcd_write_byte(0x12);   

    lcd_write_cmd(0xC4); // VDVS (C4h): VDV Set 
    lcd_write_byte(0x20);  

    lcd_write_cmd(0xC6); // FRCTRL2 (C6h): Frame Rate Control in Normal Mode 
    lcd_write_byte(0x0F);    

    lcd_write_cmd(0xD0); // PWCTRL1 (D0h): Power Control 1 
    lcd_write_byte(0xA4);
    lcd_write_byte(0xA1);

    lcd_write_cmd(0xE0); // PVGAMCTRL (E0h): Positive Voltage Gamma Control
    lcd_write_byte(0xD0);
    lcd_write_byte(0x04);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x11);
    lcd_write_byte(0x13);
    lcd_write_byte(0x2B);
    lcd_write_byte(0x3F);
    lcd_write_byte(0x54);
    lcd_write_byte(0x4C);
    lcd_write_byte(0x18);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x0B);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x23);

    lcd_write_cmd(0xE1); // NVGAMCTRL (E1h): Negative Voltage Gamma Control
    lcd_write_byte(0xD0);
    lcd_write_byte(0x04);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x11);
    lcd_write_byte(0x13);
    lcd_write_byte(0x2C);
    lcd_write_byte(0x3F);
    lcd_write_byte(0x44);
    lcd_write_byte(0x51);
    lcd_write_byte(0x2F);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x20);
    lcd_write_byte(0x23);

    lcd_write_cmd(0x20); // INVON (21h): Display Inversion On

    lcd_write_cmd(0x11); // SLPOUT (11h): Sleep Out 

    lcd_write_cmd(0x29); // DISPON (29h): Display On
}

static void lcd_config(lcd_config_t *config)
{

    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_SPI3_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_SPI3_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN0_REG, SYSTEM_SPI3_RST);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN0_REG, SYSTEM_SPI3_RST);
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_SPI3_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_SPI3_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN0_REG, SYSTEM_SPI3_DMA_RST);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN0_REG, SYSTEM_SPI3_DMA_RST);

    int div = 2;
    if (config->clk_fre == 80000000) {
        GPSPI3.clock.clk_equ_sysclk = 1;
    } else {
        GPSPI3.clock.clk_equ_sysclk = 0;
        div = 80000000 / config->clk_fre;
    }
    GPSPI3.ctrl1.clk_mode = 0;
    GPSPI3.clock.clkdiv_pre = 1 - 1;
    GPSPI3.clock.clkcnt_n = div - 1;
    GPSPI3.clock.clkcnt_l = div - 1;
    GPSPI3.clock.clkcnt_h = ((div >> 1) - 1);
    
    GPSPI3.misc.ck_dis = 0;

    GPSPI3.user1.val = 0;
    GPSPI3.slave.val = 0;
    GPSPI3.misc.ck_idle_edge = 0;
    GPSPI3.user.ck_out_edge = 0;
    GPSPI3.ctrl.wr_bit_order = 0;
    GPSPI3.ctrl.rd_bit_order = 0;
    GPSPI3.user.val = 0;
    GPSPI3.user.cs_setup = 1;
    GPSPI3.user.cs_hold = 1;
    GPSPI3.user.usr_mosi = 1;
    GPSPI3.user.usr_mosi_highpart = 0;

    GPSPI3.dma_conf.val = 0;
    GPSPI3.dma_conf.out_rst = 1;
    GPSPI3.dma_conf.out_rst = 0;
    GPSPI3.dma_conf.ahbm_fifo_rst = 1;
    GPSPI3.dma_conf.ahbm_fifo_rst = 0;
    GPSPI3.dma_conf.ahbm_rst = 1;
    GPSPI3.dma_conf.ahbm_rst = 0;
    GPSPI3.dma_out_link.dma_tx_ena = 1;
    GPSPI3.dma_conf.out_eof_mode = 1;
    GPSPI3.cmd.usr = 0;

    GPSPI3.dma_int_clr.val = ~0;
    GPSPI3.dma_int_ena.val = 0;
    GPSPI3.dma_int_ena.out_eof = 1;

    intr_handle_t intr_handle = NULL;
    esp_intr_alloc(ETS_SPI3_DMA_INTR_SOURCE, 0, lcd_isr, NULL, &intr_handle);
}

static void lcd_set_pin(lcd_config_t *config)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_clk], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin_clk, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config->pin_clk, GPIO_FLOATING);
    gpio_matrix_out(config->pin_clk, SPI3_CLK_OUT_MUX_IDX, 0, 0);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_mosi], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin_mosi, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config->pin_mosi, GPIO_FLOATING);
    gpio_matrix_out(config->pin_mosi, SPI3_D_OUT_IDX, 0, 0);

    //Initialize non-SPI GPIOs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << config->pin_dc) | (1ULL << config->pin_cs) | (1ULL << config->pin_rst) | (1ULL << config->pin_bk);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void lcd_dma_config(lcd_config_t *config) 
{
    int cnt = 0;
    lcd_obj->dma_size = LCD_DMA_MAX_SIZE;
    for (cnt = 0;;cnt++) { // 寻找可以整除dma_size的buffer大小
        if ((config->max_buffer_size - cnt) % lcd_obj->dma_size == 0) {
            break;
        }
    }
    lcd_obj->buffer_size = config->max_buffer_size - cnt;
    lcd_obj->half_buffer_size = lcd_obj->buffer_size / 2;

    lcd_obj->node_cnt = (lcd_obj->buffer_size) / lcd_obj->dma_size; // DMA节点个数
    lcd_obj->half_node_cnt = lcd_obj->node_cnt / 2;

    ESP_LOGI(TAG, "lcd_buffer_size: %d, lcd_dma_size: %d, lcd_dma_node_cnt: %d\n", lcd_obj->buffer_size, lcd_obj->dma_size, lcd_obj->node_cnt);

    lcd_obj->dma    = (lldesc_t *)heap_caps_malloc(lcd_obj->node_cnt * sizeof(lldesc_t), MALLOC_CAP_DMA);
    lcd_obj->buffer = (uint8_t *)heap_caps_malloc(lcd_obj->buffer_size * sizeof(uint8_t), MALLOC_CAP_DMA);
}

int lcd_init(lcd_config_t *config)
{
    lcd_obj = (lcd_obj_t *)heap_caps_calloc(1, sizeof(lcd_obj_t), MALLOC_CAP_DMA);
    if (!lcd_obj) {
        ESP_LOGI(TAG, "lcd object malloc error\n");
        return -1;
    }

    lcd_set_pin(config);
    lcd_config(config);
    lcd_dma_config(config);

    lcd_obj->event_queue = xQueueCreate(1, sizeof(int));
    
    lcd_obj->buffer_size = config->max_buffer_size;

    lcd_obj->pin_dc = config->pin_dc;
    lcd_obj->pin_cs = config->pin_cs;
    lcd_obj->pin_rst = config->pin_rst;
    lcd_obj->pin_bk = config->pin_bk;
    lcd_set_cs(1);

    lcd_rst();//lcd_rst before LCD Init.
    lcd_delay_ms(100);
    lcd_st7789_config(config);

    lcd_set_blk(0);
    ESP_LOGI(TAG, "lcd init ok\n");

    return 0;
}

void lcd_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    uint16_t start_pos, end_pos;
    lcd_write_cmd(0x2a);    // CASET (2Ah): Column Address Set 
    // Must write byte than byte
    if (lcd_obj->horizontal == 3) {
        start_pos = x_start + 80;
        end_pos = x_end + 80;
    } else {
        start_pos = x_start;
        end_pos = x_end;
    }
    lcd_write_byte(start_pos >> 8);
    lcd_write_byte(start_pos & 0xFF);
    lcd_write_byte(end_pos >> 8);
    lcd_write_byte(end_pos & 0xFF);

    lcd_write_cmd(0x2b);    // RASET (2Bh): Row Address Set
    if (lcd_obj->horizontal == 1) {
        start_pos = x_start + 80;
        end_pos = x_end + 80;
    } else {
        start_pos = y_start;
        end_pos = y_end;
    }
    lcd_write_byte(start_pos >> 8);
    lcd_write_byte(start_pos & 0xFF);
    lcd_write_byte(end_pos >> 8);
    lcd_write_byte(end_pos & 0xFF); 
    lcd_write_cmd(0x2c);    // RAMWR (2Ch): Memory Write 
}
