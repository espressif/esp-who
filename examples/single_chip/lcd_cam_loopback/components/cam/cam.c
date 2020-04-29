#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/lcd_cam_struct.h"
#include "soc/periph_defs.h"
#include "driver/gpio.h"
#include "soc/apb_ctrl_reg.h"
#include "esp32s3beta/rom/lldesc.h"
#include "esp32s3beta/rom/cache.h"
#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#include "cam.h"

static const char *TAG = "cam";

#define CAM_DMA_MAX_SIZE     (4095)

typedef struct {
    uint8_t *frame_buffer;
    size_t len;
} frame_buffer_queue_event_t;

typedef struct {
    uint32_t buffer_size;
    uint32_t half_buffer_size;
    uint32_t node_cnt;
    uint32_t half_node_cnt;
    uint32_t dma_size;
    uint32_t total_cnt;
    uint16_t width;
    uint16_t high;
    lldesc_t *dma;
    uint8_t *buffer;
    uint8_t *frame1_buffer;
    uint8_t *frame2_buffer;
    uint8_t frame1_buffer_en;
    uint8_t frame2_buffer_en;
    uint8_t jpeg_mode;
    QueueHandle_t event_queue;
    QueueHandle_t frame_buffer_queue;
} cam_obj_t;

static cam_obj_t *cam_obj = NULL;

void IRAM_ATTR cam_isr(void *arg)
{
    typeof(LCD_CAM.lc_dma_int_st) int_st = LCD_CAM.lc_dma_int_st;
    LCD_CAM.lc_dma_int_clr.val = int_st.val;
    BaseType_t HPTaskAwoken = pdFALSE;
    static int cnt = 0;
    if (int_st.in_suc_eof) {
        xQueueOverwriteFromISR(cam_obj->event_queue, (void *)&cnt, &HPTaskAwoken);
        cnt++;
        if (cnt == cam_obj->total_cnt) {
            cnt = 0;
        }
    }

    if (int_st.cam_vsync) {
        cnt = 0;
    }

    if(HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void cam_config(cam_config_t *config)
{
    //Enable LCD_CAM periph
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_LCD_CAM_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_LCD_CAM_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_LCD_CAM_RST);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_LCD_CAM_RST);

    LCD_CAM.cam_ctrl.val = 0;
    LCD_CAM.cam_ctrl1.val = 0;
    LCD_CAM.lc_dma_conf.val = 0;

    LCD_CAM.cam_ctrl.cam_stop_en = 0;
    LCD_CAM.cam_ctrl.cam_vsync_filter_thres = 7 - 1; // 按LCD_CAM时钟滤波
    LCD_CAM.cam_ctrl.cam_update = 0;
    LCD_CAM.cam_ctrl.cam_byte_order = 0;
    LCD_CAM.cam_ctrl.cam_bit_order = 0; 
    LCD_CAM.cam_ctrl.cam_line_int_en = 0;
    LCD_CAM.cam_ctrl.cam_vs_eof_en = config->mode.jpeg ? 1 : 0; // in_suc_eof中断产生方式

    LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = CAM_DMA_MAX_SIZE - 1; // 不能赋值为0,也容易溢出
    LCD_CAM.cam_ctrl1.cam_line_int_num = 1  - 1; // 产生hs中断的hsync个数
    LCD_CAM.cam_ctrl1.cam_clk_inv = 0;
    LCD_CAM.cam_ctrl1.cam_vsync_filter_en = 1;
    LCD_CAM.cam_ctrl1.cam_2byte_en = 0;
    LCD_CAM.cam_ctrl1.cam_de_inv = 0;
    LCD_CAM.cam_ctrl1.cam_hsync_inv = 0;
    LCD_CAM.cam_ctrl1.cam_vsync_inv = 0;
    LCD_CAM.cam_ctrl1.cam_vh_de_mode_en = 0;

	LCD_CAM.cam_ctrl.cam_clkm_div_b = 0;
	LCD_CAM.cam_ctrl.cam_clkm_div_a = 10;
	LCD_CAM.cam_ctrl.cam_clkm_div_num = 160000000 / config->xclk_fre;
	LCD_CAM.cam_ctrl.cam_clk_sel = 3;

    LCD_CAM.lc_dma_int_clr.val = 0xffffffff;
    LCD_CAM.lc_dma_int_ena.val = 0;
    intr_handle_t intr_handle = NULL;
    esp_intr_alloc(ETS_LCD_CAM_INTR_SOURCE, 0, cam_isr, NULL, &intr_handle);
}

static void cam_set_pin(cam_config_t *config)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.hsync], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.hsync, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.hsync, GPIO_FLOATING);
    gpio_matrix_in(config->pin.hsync, CAM_H_ENABLE_IDX, false);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.vsync], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.vsync, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.vsync, GPIO_FLOATING);
    gpio_matrix_in(config->pin.vsync, CAM_V_SYNC_IDX, true);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.pclk], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.pclk, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.pclk, GPIO_FLOATING);
    gpio_matrix_in(config->pin.pclk, CAM_PCLK_IDX, false);

    for(int i = 0; i < config->bit_width; i++) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_data[i]], PIN_FUNC_GPIO);
        gpio_set_direction(config->pin_data[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(config->pin_data[i], GPIO_FLOATING);
        gpio_matrix_in(config->pin_data[i], CAM_DATA_IN0_IDX + i, false);
    }

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.xclk], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.xclk, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config->pin.xclk, GPIO_FLOATING);
    gpio_matrix_out(config->pin.xclk, CAM_CLK_IDX, 0, 0);
}

void cam_stop(void)
{
    LCD_CAM.cam_dma_in_link.inlink_stop = 1;
    LCD_CAM.cam_ctrl1.cam_start = 0;
    LCD_CAM.cam_ctrl1.cam_reset = 1;
    LCD_CAM.cam_ctrl1.cam_reset = 0;
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;
    LCD_CAM.lc_dma_conf.in_rst = 1;
    LCD_CAM.lc_dma_conf.in_rst = 0;
    LCD_CAM.lc_dma_conf.ahbm_fifo_rst = 1;
    LCD_CAM.lc_dma_conf.ahbm_fifo_rst = 0;
    LCD_CAM.lc_dma_conf.ahbm_rst = 1;
    LCD_CAM.lc_dma_conf.ahbm_rst = 0;
    LCD_CAM.cam_ctrl.cam_update = 1;
    LCD_CAM.lc_dma_int_clr.in_suc_eof = 1;
    LCD_CAM.lc_dma_int_clr.cam_vsync = 1;
}

void cam_start(void)
{
    LCD_CAM.lc_dma_int_clr.in_suc_eof = 1;
    LCD_CAM.lc_dma_int_ena.in_suc_eof = 1;
    LCD_CAM.lc_dma_int_clr.cam_vsync = 1;
    LCD_CAM.lc_dma_int_ena.cam_vsync = 1;
    LCD_CAM.cam_ctrl1.cam_reset = 1;
    LCD_CAM.cam_ctrl1.cam_reset = 0;
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;
    LCD_CAM.lc_dma_conf.in_rst = 1;
    LCD_CAM.lc_dma_conf.in_rst = 0;
    LCD_CAM.lc_dma_conf.ahbm_fifo_rst = 1;
    LCD_CAM.lc_dma_conf.ahbm_fifo_rst = 0;
    LCD_CAM.lc_dma_conf.ahbm_rst = 1;
    LCD_CAM.lc_dma_conf.ahbm_rst = 0;
    LCD_CAM.cam_dma_in_link.inlink_start = 1;
    LCD_CAM.cam_ctrl.cam_update = 1;
    LCD_CAM.cam_ctrl1.cam_start = 1;
}

typedef enum {
    CAM_STATE_IDLE = 0,
    CAM_STATE_READ_BUF1 = 1,
    CAM_STATE_READ_BUF2 = 2,
} cam_state_t;

//Copy fram from DMA buffer to fram buffer
static void cam_task(void *arg)
{
    int state = CAM_STATE_IDLE;
    int cnt = 0;
    frame_buffer_queue_event_t frame_buffer_queue_event = {0};

    while (1) {
        xQueueReceive(cam_obj->event_queue, (void *)&cnt, portMAX_DELAY);
        if (cam_obj->jpeg_mode) {
            lldesc_t *in_suc_eof_dma = (lldesc_t *)LCD_CAM.lc_in_suc_eof_des_addr;
            int total_len = (in_suc_eof_dma->buf - cam_obj->buffer) + in_suc_eof_dma->length;
            cam_stop();
            frame_buffer_queue_event.len = total_len;
            if (cam_obj->frame1_buffer_en) {
                memcpy(&cam_obj->frame1_buffer[0], &cam_obj->buffer[0], total_len);
                frame_buffer_queue_event.frame_buffer = cam_obj->frame1_buffer;
                cam_obj->frame1_buffer_en = 0;
                xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_queue_event, portMAX_DELAY);
            } else if (cam_obj->frame2_buffer_en) {
                memcpy(&cam_obj->frame2_buffer[0], &cam_obj->buffer[0], total_len);
                frame_buffer_queue_event.frame_buffer = cam_obj->frame2_buffer;
                cam_obj->frame2_buffer_en = 0;
                xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_queue_event, portMAX_DELAY);
            }
            cam_start();
        } else {
            switch (state) {
                case CAM_STATE_IDLE: {
                    if (cnt == 0) {
                        if (cam_obj->frame1_buffer_en) {
                            state = 1;
                            memcpy(&cam_obj->frame1_buffer[cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);
                        } else if (cam_obj->frame2_buffer_en) {
                            state = 2;
                            memcpy(&cam_obj->frame2_buffer[cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);
                        } else {
                            continue;
                        }
                    } else {
                        continue;
                    }
                }
                break;
                case CAM_STATE_READ_BUF1: {
                    memcpy(&cam_obj->frame1_buffer[cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);
                    if (cnt == cam_obj->total_cnt - 1) {
                        cam_obj->frame1_buffer_en = 0;
                        frame_buffer_queue_event.frame_buffer = cam_obj->frame1_buffer;
                        frame_buffer_queue_event.len = cam_obj->width * cam_obj->high * 2;
                        xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_queue_event, portMAX_DELAY);
                        state = 0;
                    }
                }
                break;

                case CAM_STATE_READ_BUF2: {
                    memcpy(&cam_obj->frame2_buffer[cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);
                    if (cnt == cam_obj->total_cnt - 1) {
                        cam_obj->frame2_buffer_en = 0;
                        frame_buffer_queue_event.frame_buffer = cam_obj->frame2_buffer;
                        frame_buffer_queue_event.len = cam_obj->width * cam_obj->high * 2;
                        xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_queue_event, portMAX_DELAY);
                        state = 0;
                    }
                }
                break;
            }
        }
    }
}

size_t cam_take(uint8_t **buffer_p)
{
    frame_buffer_queue_event_t frame_buffer_queue_event;
    xQueueReceive(cam_obj->frame_buffer_queue, (void *)&frame_buffer_queue_event, portMAX_DELAY);
    *buffer_p = frame_buffer_queue_event.frame_buffer;
    return frame_buffer_queue_event.len;
}

void cam_give(uint8_t *buffer)
{
    if (buffer == cam_obj->frame1_buffer) {
        cam_obj->frame1_buffer_en = 1;
    } else if (buffer == cam_obj->frame2_buffer){
        cam_obj->frame2_buffer_en = 1;
    }
}

void cam_dma_config(cam_config_t *config) 
{
    int cnt = 0;
    if (config->mode.jpeg) {
        cam_obj->buffer_size = config->max_buffer_size;
        cam_obj->half_buffer_size = cam_obj->buffer_size / 2;
        cam_obj->dma_size = CAM_DMA_MAX_SIZE;
    } else {
        if (config->max_buffer_size / 2.0 > 16384) { // must less than max(cam_rec_data_bytelen)
            config->max_buffer_size = 16384 * 2;
        }
        for (cnt = 0;;cnt++) { // 寻找可以整除的buffer大小
            if ((config->size.width * config->size.high * 2) % (config->max_buffer_size - cnt) == 0) {
                break;
            }
        }
        cam_obj->buffer_size = config->max_buffer_size - cnt;

        cam_obj->half_buffer_size = cam_obj->buffer_size / 2;
        for (cnt = 0;;cnt++) { // 寻找可以整除的dma大小
            if ((cam_obj->half_buffer_size) % (CAM_DMA_MAX_SIZE - cnt) == 0) {
                break;
            }
        }
        cam_obj->dma_size = CAM_DMA_MAX_SIZE - cnt;
    }

    cam_obj->node_cnt = (cam_obj->buffer_size) / cam_obj->dma_size; // DMA节点个数
    cam_obj->half_node_cnt = cam_obj->node_cnt / 2;
    cam_obj->total_cnt = (config->size.width * config->size.high * 2) / cam_obj->half_buffer_size; // 产生中断拷贝的次数, 乒乓拷贝

    ESP_LOGI(TAG, "cam_buffer_size: %d, cam_dma_size: %d, cam_dma_node_cnt: %d, cam_total_cnt: %d\n", cam_obj->buffer_size, cam_obj->dma_size, cam_obj->node_cnt, cam_obj->total_cnt);

    cam_obj->dma    = (lldesc_t *)heap_caps_malloc(cam_obj->node_cnt * sizeof(lldesc_t), MALLOC_CAP_DMA);
    cam_obj->buffer = (uint8_t *)heap_caps_malloc(cam_obj->buffer_size * sizeof(uint8_t), MALLOC_CAP_DMA);

    for (int x = 0; x < cam_obj->node_cnt; x++) {
        cam_obj->dma[x].size = cam_obj->dma_size;
        cam_obj->dma[x].length = cam_obj->dma_size;
        cam_obj->dma[x].eof = 0;
        cam_obj->dma[x].owner = 1;
        cam_obj->dma[x].buf = (cam_obj->buffer + cam_obj->dma_size * x);
        cam_obj->dma[x].empty = &cam_obj->dma[(x + 1) % cam_obj->node_cnt];
    }

    LCD_CAM.cam_dma_in_link.inlink_addr = ((uint32_t)&cam_obj->dma[0]) & 0xfffff;
    if (config->mode.jpeg == 0) {
        LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = cam_obj->half_buffer_size - 1; // 乒乓操作
    }
}

int cam_init(const cam_config_t *config)
{
    cam_obj = (cam_obj_t *)heap_caps_calloc(1, sizeof(cam_obj_t), MALLOC_CAP_DMA);
    if (!cam_obj) {
        ESP_LOGI(TAG, "camera object malloc error\n");
        return -1;
    }
    memset(cam_obj, 0, sizeof(cam_obj_t));
    cam_obj->width = config->size.width;
    cam_obj->high = config->size.high;
    cam_obj->frame1_buffer = config->frame1_buffer;
    cam_obj->frame2_buffer = config->frame2_buffer;
    cam_obj->jpeg_mode = config->mode.jpeg;
    cam_set_pin(config);
    cam_config(config);
    cam_dma_config(config);

    cam_obj->event_queue = xQueueCreate(1, sizeof(int));
    cam_obj->frame_buffer_queue = xQueueCreate(2, sizeof(frame_buffer_queue_event_t));

    if (cam_obj->frame1_buffer != NULL) {
        cam_obj->frame1_buffer_en = 1;
    } else {
        cam_obj->frame1_buffer_en = 0;
    }
    
    if (cam_obj->frame2_buffer != NULL) {
        cam_obj->frame2_buffer_en = 1;
    } else {
        cam_obj->frame2_buffer_en = 0;
    }
    xTaskCreate(cam_task, "cam_task", 1024 * 4, NULL, config->task_pri, NULL);
    return 0;
}