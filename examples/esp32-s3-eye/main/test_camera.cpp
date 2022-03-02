#include "test_logic.h"
#include "who_camera.h"
#include "human_face_detect_msr01.hpp"

static const char *TAG = "CAMERA";

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;

#define MAX_TEST_TIMES 60
static int fb_count = 1;

static esp_err_t camera_init()
{
    static bool initialized = false;
    if (initialized)
    {
        return ESP_OK;
    }
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sscb_sda = CAMERA_PIN_SIOD;
    config.pin_sscb_scl = CAMERA_PIN_SIOC;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = 12000000;
    config.pixel_format = PIXFORMAT_RGB565;
    config.frame_size = FRAMESIZE_240X240;
    config.jpeg_quality = 12;
    config.fb_count = fb_count;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        ESP_LOGE(TAG, "--------------- CAMERA Test FAIL ---------------\n");
        return err;
    }

    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1); //flip it back
    //initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_brightness(s, 1);  //up the blightness just a bit
        s->set_saturation(s, -2); //lower the saturation
    }
    initialized = true;

    return ESP_OK;
}

static void camera_test_task(void *arg)
{
    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        xQueueReceive(queues_tests[TEST_CAMERA], &g_state_test, portMAX_DELAY);
        bool camera_pass = false;
        if (g_state_test == TEST_CAMERA)
        {
            esp_err_t ret = camera_init();

            if (ESP_OK != ret)
            {
                camera_pass = false;
                ESP_LOGE(TAG, "--------------- CAMERA Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &camera_pass, portMAX_DELAY);
                continue;
            }

            ESP_LOGW(TAG, "Please place the camera in front of the human face\n");

            for (int i = 0; i < MAX_TEST_TIMES; ++i)
            {
                camera_fb_t *frame = NULL;
                while (frame == NULL)
                    frame = esp_camera_fb_get();    //这一帧图像有可能是上次测试camera时采到的图像。
                esp_camera_fb_return(frame);
                frame = NULL;
                while (frame == NULL) 
                    frame = esp_camera_fb_get();  //确保camera拿到的图像为当前帧。

                std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                esp_camera_fb_return(frame);

                if (detect_results.size() > 0)
                {
                    camera_pass = true;
                    ESP_LOGI(TAG, "--------------- CAMERA Test PASS ---------------\n");
                    xQueueSend(queue_test_result, &camera_pass, portMAX_DELAY);
                    break;
                }
            }

            if (!camera_pass)
            {
                ESP_LOGE(TAG, "--------------- CAMERA Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &camera_pass, portMAX_DELAY);
            }
        }
    }
}

void register_camera_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue)
{
    queue_test_result = result_queue;
    queues_tests = test_queues;

    xTaskCreatePinnedToCore(&camera_test_task, "camera_test_task", 4 * 1024, NULL, 5, NULL, 0);
}