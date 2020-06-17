#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/system_reg.h"
#include "esp_timer.h"
#include "cam.h"
#include "ov2640.h"
#include "ov3660.h"
#include "sensor.h"
#include "lcd.h"
#include "jpeg.h"
#include "lssh_forward.h"
#include "pe_forward.h"
#include "image_util.h"
#include "input.h"
#include "lssh_human_face_mn2_q.h"
#include "fb_gfx.h"
#include "driver/uart.h"
#include "zbar.h"

static const char *TAG = "main";
static char *GET_FLAG = "get";
static char *START = "start";

#define JPEG_MODE 0

#define CAM_WIDTH   (320)
#define CAM_HIGH    (240)

#define LCD_CLK   GPIO_NUM_15
#define LCD_MOSI  GPIO_NUM_9
#define LCD_DC    GPIO_NUM_13
#define LCD_RST   GPIO_NUM_16
#define LCD_CS    GPIO_NUM_11
#define LCD_BK    GPIO_NUM_6

#define CAM_XCLK  GPIO_NUM_4
#define CAM_PCLK  GPIO_NUM_1
#define CAM_VSYNC GPIO_NUM_2
#define CAM_HSYNC GPIO_NUM_3

#define CAM_D0    GPIO_NUM_10
#define CAM_D1    GPIO_NUM_12
#define CAM_D2    GPIO_NUM_41
#define CAM_D3    GPIO_NUM_42
#define CAM_D4    GPIO_NUM_39
#define CAM_D5    GPIO_NUM_40
#define CAM_D6    GPIO_NUM_21
#define CAM_D7    GPIO_NUM_38

#define CAM_SCL   GPIO_NUM_7
#define CAM_SDA   GPIO_NUM_8

#define ECHO_TEST_TXD  (1)
#define ECHO_TEST_RXD  (3)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)


//box_array_t *onet_forward(dl_matrix3du_t *image, box_array_t *net_boxes, net_config_t *config);
//face_id_list id_list = {0};

static void rgb_print(dl_matrix3du_t *image_matrix, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR565;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(image_matrix, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

void draw_rectangle_od_rgb565(uint16_t *buf, od_box_array_t *boxes, int width, int height)
{ /*{{{*/
    uint16_t p[14];
    for (int i = 0; i < boxes->len; i++)
    {
        // rectangle box
        for (int j = 0; j < 4; j++)
            p[j] = (uint16_t)boxes->box[i].box_p[j];

        // landmark
        // for (int j = 0; j < 10; j++)
        //     p[j + 4] = (uint16_t)boxes->landmark[i].landmark_p[j];

        if ((p[2] < p[0]) || (p[3] < p[1]))
            return;
        p[0] = max(0, p[0]);
        p[1] = max(0, p[1]);
        p[2] = min(width-1, p[2]);
        p[3] = min(height-1, p[3]);

#define RGB565_GREEN_REVERSE 0xE007
#define RGB565_RED_REVERSE 0xF8
        // rectangle box
        for (int w = p[0]; w < p[2] + 1; w++)
        {
            int x1 = (p[1] * width + w);
            int x2 = (p[3] * width + w);
            buf[x1] = RGB565_GREEN_REVERSE;
            buf[x2] = RGB565_GREEN_REVERSE;
        }
        for (int h = p[1]; h < p[3] + 1; h++)
        {
            int y1 = (h * width + p[0]);
            int y2 = (h * width + p[2]);
            buf[y1] = RGB565_GREEN_REVERSE;
            buf[y2] = RGB565_GREEN_REVERSE;
        }

        // landmark
#if 0
        for (int j = 0; j < 10; j += 2)
        {
            int x = p[j + 5] * width + p[j + 4];
            buf[x] = RGB565_RED_REVERSE;
            buf[x + 1] = RGB565_RED_REVERSE;
            buf[x + 2] = RGB565_RED_REVERSE;

            buf[width + x] = RGB565_RED_REVERSE;
            buf[width + x + 1] = RGB565_RED_REVERSE;
            buf[width + x + 2] = RGB565_RED_REVERSE;

            buf[2 * width + x] = RGB565_RED_REVERSE;
            buf[2 * width + x + 1] = RGB565_RED_REVERSE;
            buf[2 * width + x + 2] = RGB565_RED_REVERSE;
        }
#endif
    }
} /*}}}*/

void draw_landmarks_rgb565(uint16_t *buf, dl_matrix3d_t *landmarks, int width, int height)
{ /*{{{*/
    int count = 0;
    int len = width*height;
    for (int i = 0; i < landmarks->n; i++)
    {
        for(int j=0;j<landmarks->h;j++){
            int x = (int)(landmarks->item[count+1]) * width + (int)(landmarks->item[count]);
            if((x+2)<len){
                buf[x] = RGB565_RED_REVERSE;
                buf[x + 1] = RGB565_RED_REVERSE;
                buf[x + 2] = RGB565_RED_REVERSE;
            }
            x = x + width;
            if((x+2)<len){
                buf[x] = RGB565_RED_REVERSE;
                buf[x + 1] = RGB565_RED_REVERSE;
                buf[x + 2] = RGB565_RED_REVERSE;
            }
            x = x + width;
            if((x+2)<len){
                buf[x] = RGB565_RED_REVERSE;
                buf[x + 1] = RGB565_RED_REVERSE;
                buf[x + 2] = RGB565_RED_REVERSE;
            }
            count += 2;
        }
    }
} /*}}}*/

typedef struct
{
    float score;          /// score threshold for filter candidates by score
    float nms;            /// nms threshold for nms process
    int candidate_number; /// candidate number limitation for each net
} threshold_config_t;
typedef struct
{
    int w;                        /// net width
    int h;                        /// net height
    threshold_config_t threshold; /// threshold of net
} net_config_t;


void hand_detection(uint16_t *cam_buf, hd_config_t hd_config) {
    int t1 = esp_timer_get_time();
    int32_t c1 = get_count();
    dl_matrix3dq_t *image = dl_matrix3dq_alloc(1, hd_config.target_size, hd_config.target_size, 3, -10);
    // image_resize_n_shift(image->item, cam_buf, 320/4, 240/4, 3, 320, 4, 2);
    image_resize_shift_fast(image->item, cam_buf, hd_config.target_size, 3, 320, 240, hd_config.target_size, hd_config.target_size*240/320, 2);
    od_box_array_t *hd_boxes = hand_detection_forward(image, hd_config);
    // dl_matrix3du_t *image = dl_matrix3du_alloc(1, 320, 240, 3);
    // transform_input_image(image->item, cam_buf, 240*320);
    // dl_matrix3dq_t *hd_image_resize = od_image_preporcess(image->item, 320, 240, hd_config.target_size, -10, 0);
    // // printf("input size: %d, %d, %d, %d\n", hd_image_resize->n, hd_image_resize->h,hd_image_resize->w,hd_image_resize->c);
    // od_box_array_t *hd_boxes = hand_detection_forward(hd_image_resize, hd_config);
    if (hd_boxes)
    {
        draw_rectangle_od_rgb565(cam_buf, hd_boxes, CAM_WIDTH, CAM_HIGH);
        // dl_matrix3d_t *hand_landmarks = handpose_estimation_forward2(cam_buf, hd_boxes, 112, 320, 240, 2);
        // dl_matrix3d_t *hand_landmarks = handpose_estimation_forward(image, 112, hd_boxes, 2);
        // draw_landmarks_rgb565(cam_buf, hand_landmarks, CAM_WIDTH, CAM_HIGH);
        // dl_matrix3d_free(hand_landmarks);
        dl_lib_free(hd_boxes->cls);
        dl_lib_free(hd_boxes->score);
        dl_lib_free(hd_boxes->box);
        dl_lib_free(hd_boxes);
    }
    // dl_matrix3du_free(image);
    // pe_test2();
}


bool rgb565togray(uint8_t* rgb_buf, uint8_t* gray_buf, int len)
{
    int r,g,b,y;
    uint8_t hb, lb;
    for(int k=0; k<len; k++){
        hb = *rgb_buf++;
        lb = *rgb_buf++;
        b = (lb & 0x1F) << 3;
        g = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        r = hb & 0xF8;

        // r = ((*(int16_t *)rgb_buf) & 0xF800) >> 8;
        // g = ((*(int16_t *)rgb_buf) & 0x07E0) >> 3;   
        // b = ((*(int16_t *)rgb_buf) & 0x001F) << 3;  


        y = (r*77)+(g*151)+(b*28);
         *gray_buf++ = (y>>8);

        // gray_buf[k] = 0.3 * rgb_buf[k * 3] + 0.59 * rgb_buf[k * 3 + 1] + 0.11 * rgb_buf[k * 3 + 2];
    }
    return true;
}


static void cam_task(void *arg)
{
#if 1
    lcd_config_t lcd_config = {
        .clk_fre = 80 * 1000 * 1000,
        .pin_clk = LCD_CLK,
        .pin_mosi = LCD_MOSI,
        .pin_dc = LCD_DC,
        .pin_cs = LCD_CS,
        .pin_rst = LCD_RST,
        .pin_bk = LCD_BK,
        .max_buffer_size = 32 * 1024,
        .horizontal = 2 // 2: UP, 3： DOWN
    };
    lcd_init(&lcd_config);
#endif

    //face_id_init(&id_list, 1, 3);

#if 0

    uint8_t *buf = heap_caps_malloc(CAM_WIDTH * CAM_HIGH * 2, MALLOC_CAP_SPIRAM);

    while (1)
    {
        memcpy(buf, get_input(), CAM_WIDTH * CAM_HIGH * 2);
        face_detection(buf);
        lcd_set_index(0, 0, CAM_WIDTH - 1, CAM_HIGH - 1);
        lcd_write_data(buf, CAM_WIDTH * CAM_HIGH * 2);
        vTaskDelay(100);
    }
    
#else
    cam_config_t cam_config = {
        .bit_width = 8,
        .mode.jpeg = JPEG_MODE,
        .xclk_fre = 16 * 1000 * 1000,
        .pin = {
            .xclk  = CAM_XCLK,
            .pclk  = CAM_PCLK,
            .vsync = CAM_VSYNC,
            .hsync = CAM_HSYNC,
        },
        .pin_data = {CAM_D0, CAM_D1, CAM_D2, CAM_D3, CAM_D4, CAM_D5, CAM_D6, CAM_D7},
        .vsync_invert = true,
        .hsync_invert = false,
        .size = {
            .width = CAM_WIDTH,
            .high  = CAM_HIGH,
        },
        .max_buffer_size = 32 * 1024,
        .task_stack = 1024,
        .task_pri = configMAX_PRIORITIES
    };
    // 使用PingPang buffer，帧率更高， 也可以单独使用一个buffer节省内存
    cam_config.frame1_buffer = (uint8_t *)heap_caps_malloc(CAM_WIDTH * CAM_HIGH * 2 * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    cam_config.frame2_buffer = (uint8_t *)heap_caps_malloc(CAM_WIDTH * CAM_HIGH * 2 * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    

    cam_config_t cam_config_sl = {
        .bit_width = 8,
        .mode.jpeg = JPEG_MODE,
        .xclk_fre = 4 * 1000 * 1000,
        .pin = {
            .xclk  = CAM_XCLK,
            .pclk  = CAM_PCLK,
            .vsync = CAM_VSYNC,
            .hsync = CAM_HSYNC,
        },
        .pin_data = {CAM_D0, CAM_D1, CAM_D2, CAM_D3, CAM_D4, CAM_D5, CAM_D6, CAM_D7},
        .vsync_invert = true,
        .hsync_invert = false,
        .size = {
            .width = CAM_WIDTH,
            .high  = CAM_HIGH,
        },
        .max_buffer_size = 32 * 1024,
        .task_stack = 1024,
        .task_pri = configMAX_PRIORITIES
    };
    // 使用PingPang buffer，帧率更高， 也可以单独使用一个buffer节省内存
    cam_config_sl.frame1_buffer = (uint8_t *)heap_caps_malloc(CAM_WIDTH * CAM_HIGH * 2 * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    cam_config_sl.frame2_buffer = (uint8_t *)heap_caps_malloc(CAM_WIDTH * CAM_HIGH * 2 * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    

    
    cam_init(&cam_config);

    sensor_t sensor;
    SCCB_Init(CAM_SDA, CAM_SCL);
    sensor.slv_addr = SCCB_Probe();
    ESP_LOGI(TAG, "sensor_id: 0x%x\n", sensor.slv_addr);

    int detect_flag = 0;

    if (sensor.slv_addr == 0x30) { // OV2640
        // cam_init(&cam_config);

        if (OV2640_Init(0, 1) != 0) {
            goto fail;
        }
        if (cam_config.mode.jpeg) {
            OV2640_JPEG_Mode();
        } else {
            OV2640_RGB565_Mode(false);	//RGB565模式
        }
        
        OV2640_ImageSize_Set(800, 600);
        OV2640_ImageWin_Set(0, 0, 800, 600);
        OV2640_OutSize_Set(CAM_WIDTH, CAM_HIGH); 
        detect_flag = 1;
    } else if (sensor.slv_addr == 0x3C) { // OV3660
        cam_deinit();
        cam_init(&cam_config_sl);

        ov3660_init(&sensor);
        sensor.init_status(&sensor);
        if (sensor.reset(&sensor) != 0) {
            goto fail;
        }
        if (cam_config.mode.jpeg) {
            sensor.set_pixformat(&sensor, PIXFORMAT_JPEG);
        } else {
            sensor.set_pixformat(&sensor, PIXFORMAT_RGB565);
        }
        // sensor.set_framesize(&sensor, FRAMESIZE_QVGA);
        sensor.set_res_raw(&sensor, 0, 0, 2079, 1547, 8, 2, 1920, 800, CAM_WIDTH, CAM_HIGH, true, true);
        sensor.set_vflip(&sensor, 1);
        sensor.set_hmirror(&sensor, 1);
        sensor.set_pll(&sensor, false, 15, 1, 0, false, 0, true, 5); // 39 fps
    } else {
        ESP_LOGE(TAG, "sensor is temporarily not supported\n");
        goto fail;
    }

    
    ESP_LOGI(TAG, "camera init done\n");
    vTaskDelay(100 / portTICK_RATE_MS);
    cam_start();

    uart_config_t uart_config = 
    {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    //send buffer
    uint8_t *cache = (uint8_t *) malloc(BUF_SIZE * 15);
    //add start flag.
    memcpy(cache, START, 5);


    dl_matrix3dq_op_init();

    //dl_matrix3du_t *image_mat = dl_matrix3du_alloc(1, CAM_WIDTH, CAM_HIGH, 3);
    int n = 3;

    // lssh_config_t lssh_config = lssh_get_config(lssh_human_face_mn2, 16*n, 10, 0.3, 240, 320, false, DL_TIE_IMPL);
    // dl_matrix3dq_t *image_mat = dl_matrix3dq_alloc(1, 240/n, 320/n, 3, 0);

    uint32_t fb_len;
    fb_data_t fb;
    fb.width = 320;
    fb.height = 240;
    fb.bytes_per_pixel = 2;
    fb_len = fb.width*fb.height*fb.bytes_per_pixel;

    


    int last_time = esp_timer_get_time();

    /* Load configuration for detection */
    while (1) {
        // printf("Start free RAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
        // printf("SPI RAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM));
        // printf("SRAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL));
        uint8_t *cam_buf = NULL;
        size_t recv_len = cam_take(&cam_buf);

        // if(detect_flag){
        //     int t1 = esp_timer_get_time();
        //     // face_detection(image_mat, cam_buf, lssh_config, n);
        //     hand_detection(cam_buf, hd_config);
        //     int t2 = esp_timer_get_time();
        //     fb.data = cam_buf;
        //     int fps = 1e6 / (t2 - t1);
        //     char buf[10];
        //     // snprintf(buf, 10, "FPS: %d\n", fps);
        //     // fb_gfx_print(&fb, 20, 20, RGB565_MASK_GREEN, buf);
        //     last_time = t2;
        // }else{
        //     zbar_image_scanner_t *scanner = NULL;
        //     scanner = zbar_image_scanner_create();

        //     /* configure the reader */
        //     zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

        //     int width = fb.width;
        //     int height = fb.height;
        //     int len = width * height;
        //     uint8_t *image_data = heap_caps_malloc(len, MALLOC_CAP_SPIRAM);
        //     // memcpy(image_data, qrcode_png, width * height);

        //     rgb565togray(cam_buf, image_data, len);
        //     // memcpy(image_data, fb.buf, fb.width * fb.height);


        //     zbar_image_t *image = zbar_image_create();
        //     zbar_image_set_format(image, *(int*)"GREY");
        //     zbar_image_set_size(image, width, height);
        //     zbar_image_set_data(image, image_data, width * height, zbar_image_free_data);
            
        //     // scan the image for barcodes
        //     int n = zbar_scan_image(scanner, image);

        //     // extract results 
        //     const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        //     for(; symbol; symbol = zbar_symbol_next(symbol)) {
        //         // do something useful with results 
        //         zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        //         const char *data = zbar_symbol_get_data(symbol);

        //         printf("decoded %s symbol \"%s\"\n\n",
        //             zbar_get_symbol_name(typ), data);
        //         // ESP_LOGI(TAG, "Scan image in %d ms.", (int)(esp_timer_get_time() - init_time) / 1000);
        //     }
        //     // clean up
        //     zbar_image_destroy(image);
        //     zbar_image_scanner_destroy(scanner);
        //     free(image_data);
        // }

        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 100 / portTICK_RATE_MS);
        if (len > 0) 
        {
            if (strncmp((char *)data, GET_FLAG, 3) == 0) 
            {
                    printf("get start flag \n");
                    memcpy(cache + 5, &fb_len, 4);
                    // memcpy(cache + 5 + 4, cam_buf, fb_len);
                    uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4);
                    uart_write_bytes(UART_NUM_1, (char *)cam_buf, fb_len);
            }
        }
        
#if JPEG_MODE
        int w, h;
        uint8_t *img = jpeg_decode(cam_buf, &w, &h);
        if (img) {
            printf("jpeg: w: %d, h: %d\n", w, h);
            lcd_set_index(0, 0, w - 1, h - 1);
            lcd_write_data(img, w * h * sizeof(uint16_t));
            free(img);
        }
#else
        lcd_set_index(0, 0, CAM_WIDTH - 1, CAM_HIGH - 1);
        lcd_write_data(cam_buf, CAM_WIDTH * CAM_HIGH * 2);
#endif
        cam_give(cam_buf);   
        // int t3 = esp_timer_get_time();
        // printf("time: %d + %d = %dms, fps: %d, %d\n",
        //         (t2 - t1) / 1000,
        //         (t3 - t2) / 1000,
        //         (t3 - t1) / 1000,
        //         1000000 / (t3 - t1), fps);
        // 使用逻辑分析仪观察帧率
        gpio_set_level(LCD_BK, 1);
        gpio_set_level(LCD_BK, 0); 
 
    }
#endif
    //dl_matrix3du_free(image_mat);
fail:
    free(cam_config.frame1_buffer);
    free(cam_config.frame2_buffer);
    cam_deinit();
    vTaskDelete(NULL);
}

void app_main() 
{
    // char *tasklist = malloc(1024);
    SET_PERI_REG_MASK(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON); // fix two core issue
    xTaskCreatePinnedToCore(cam_task, "cam_task", 2*4096, NULL, 5, NULL, 1);
    // while(1){
    //     vTaskGetRunTimeStats(tasklist);
    //     vTaskDelay(1000);
    // }
}
