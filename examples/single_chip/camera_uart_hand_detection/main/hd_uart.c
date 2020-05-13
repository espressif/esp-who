#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_camera.h"
#include "driver/uart.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "driver/ledc.h"
#include "pe_forward.h"

#define ECHO_TEST_TXD  (1)
#define ECHO_TEST_RXD  (3)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)

#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED 0x000000FF
#define FACE_COLOR_GREEN 0x0000FF00
#define FACE_COLOR_BLUE 0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

static char *GET_FLAG = "get";
static char *START = "start";


static void draw_boxes(dl_matrix3du_t *image_matrix, od_box_array_t *boxes)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_GREEN;
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++)
    {
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
    }
}


void task_output(void *pvParameters)
{
    uint64_t start_time;
    uint64_t end_time;
    uint32_t fb_len;
    camera_fb_t *fb;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;

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

    do
    {   
        start_time = esp_timer_get_time();
        fb = esp_camera_fb_get();
        if (fb == NULL)
        {
            continue;
        }
        end_time = esp_timer_get_time();

        start_time = (end_time - start_time) / 1000;

        fb_len = fb->len;
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;

        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 100 / portTICK_RATE_MS);
        if (len > 0) 
        {
            if (strncmp((char *)data, GET_FLAG, 3) == 0) 
            {
                dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);

                if (!image_matrix)
                {
                    //ESP_LOGE("dl_matrix3du_alloc failed");
                    memcpy(cache + 5, &fb_len, 4);
                    memcpy(cache + 5 + 4, fb->buf, fb_len);
                    uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + fb_len);
                }
                else
                {
                    if (!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item))
                    {
                        dl_matrix3du_free(image_matrix);
                        //ESP_LOGE("fmt2rgb888 failed");
                        memcpy(cache + 5, &fb_len, 4);
                        memcpy(cache + 5 + 4, fb->buf, fb_len);
                        uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + fb_len);
                    }else{
                        od_box_array_t *detect_result = hand_detection_forward(image_matrix, 80, 0.4, 0.45, 1);

                        if(detect_result){
                            draw_boxes(image_matrix, detect_result);
                            dl_lib_free(detect_result->score);
                            dl_lib_free(detect_result->box);
                            dl_lib_free(detect_result->cls);
                            dl_lib_free(detect_result);
                            if (!fmt2jpg(image_matrix->item, fb->width * fb->height * 3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len))
                            {
                                dl_matrix3du_free(image_matrix);
                                //ESP_LOGE(TAG, "fmt2jpg failed");
                                memcpy(cache + 5, &fb_len, 4);
                                memcpy(cache + 5 + 4, fb->buf, fb_len);
                                uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + fb_len);
                                continue;
                            }
                            dl_matrix3du_free(image_matrix);
                            memcpy(cache + 5, &_jpg_buf_len, 4);
                            memcpy(cache + 5 + 4, _jpg_buf, _jpg_buf_len);
                            uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + _jpg_buf_len);
                        }else{
                            dl_matrix3du_free(image_matrix);
                            memcpy(cache + 5, &fb_len, 4);
                            memcpy(cache + 5 + 4, fb->buf, fb_len);
                            uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + fb_len);
                        }
                                   
                    }
                }
                
            }
        }
        esp_camera_fb_return(fb);
    } while (1);
   
}

void app_uart_image_main()
{
    xTaskCreatePinnedToCore(task_output, "uart_process", 4 * 1024, NULL, 5, NULL, 1);
}
