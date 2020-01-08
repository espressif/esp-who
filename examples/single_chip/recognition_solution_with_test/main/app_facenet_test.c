#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_facenet_test.h"
#include "sdkconfig.h"

static const char *TAG = "CAMERA TEST";
int boot_key_flag = 0;
int func_key_flag = 0; 

/*static mtmn_config_t mtmn_init_config_()
{
    mtmn_config_t mtmn_config;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.7;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.r_threshold.score = 0.6;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 4;
    mtmn_config.o_threshold.score = 0.6;
    mtmn_config.o_threshold.nms = 0.4;
    mtmn_config.o_threshold.candidate_number = 1;

    return mtmn_config;
}*/


void picture_quality_detect (void *arg)
{/*{{{*/
    size_t frame_num = 0;
    size_t detected_frame_num = 0;
    size_t blur_frame_num = 0;
    float blur_thresh = 7700.0;
    float blur_score = 0;
    dl_matrix3du_t *image_matrix = NULL;
    camera_fb_t *fb = NULL;
    mtmn_config_t mtmn_config = mtmn_init_config();

    do
    {
        if(boot_key_flag == 1){
            ESP_LOGI("BOOT KEY","PASS\n");
            boot_key_flag = 0;
            g_state_test = WAIT_CAMERA_TEST;
        }
        if(func_key_flag == 1){
            ESP_LOGI("FUNC KEY","PASS\n");
            func_key_flag = 0;
        }
    	if(g_state_test != CAMERA_TEST){
    		vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
    	}
        //int64_t start_time = esp_timer_get_time();
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        //int64_t fb_get_time = esp_timer_get_time();
        //ESP_LOGI(TAG, "Get one frame in %lld ms.", (fb_get_time - start_time) / 1000);

        image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
        uint32_t res = fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
        if (true != res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            continue;
        }

        esp_camera_fb_return(fb);

        box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        frame_num++;
        //ESP_LOGI(TAG, "Detection time consumption: %lldms", (esp_timer_get_time() - fb_get_time) / 1000);

        dl_matrix3du_t *gray_image = bgr2gray(image_matrix);
        /*blur_score = tenengrad_score(gray_image);
        printf("tenengrad_score: %f\n",blur_score);*/
        blur_score = laplace_score(gray_image);
        ESP_LOGI(TAG,"laplace_score: %f, threshold: %f",blur_score,blur_thresh);
        if(blur_score > blur_thresh)
            blur_frame_num++;

        if (net_boxes)
        {
            detected_frame_num++;
            ESP_LOGI(TAG, "FRAME: %d,DETECTED: %d\n", frame_num,detected_frame_num);
            dl_lib_free(net_boxes->score);
            dl_lib_free(net_boxes->box);
            dl_lib_free(net_boxes->landmark);
            dl_lib_free(net_boxes);
        }
        else{
        	ESP_LOGI(TAG, "FRAME: %d,DETECT FAIL\n", frame_num);
        }
        dl_matrix3du_free(gray_image);
        dl_matrix3du_free(image_matrix);
        //g_state = WAIT_CAMERA_TEST;

        if((detected_frame_num >= 1)&&(blur_frame_num >= 1)&&(frame_num <= 8)){
            ESP_LOGI(TAG, "PASS\n");
            if((led_pass == 1)&&(mic_pass == 1)){
                g_state_test = TEST_PASS;
            }else{
                g_state_test = WAIT_CAMERA_TEST;
            }
            
        }
        else if(frame_num == 8){
            frame_num = 0;
            blur_frame_num = 0;
            detected_frame_num = 0;
            ESP_LOGE(TAG, "FAIL\n");
            g_state_test = WAIT_CAMERA_TEST;
        }

    } while(1);
}/*}}}*/

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    if(g_state_test == WAIT_CAMERA_TEST)
    {
    	g_state_test = CAMERA_TEST;
        func_key_flag = 1;

    }
}

static void IRAM_ATTR boot_isr_handler(void* arg)
{
    if(g_state_test == WAIT_KEY_TEST)
    {
        boot_key_flag = 1;
    }
}

void app_facenet_test_main()
{
    gpio_install_isr_service(0);
    gpio_set_intr_type(15, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(15, gpio_isr_handler, (void*) 15);
    gpio_set_intr_type(0, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(0, boot_isr_handler, (void*) 0);

    xTaskCreatePinnedToCore(picture_quality_detect, "process", 4 * 1024, NULL, 5, NULL, 1);
}