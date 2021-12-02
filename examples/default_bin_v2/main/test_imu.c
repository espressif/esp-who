#include "test_logic.h"
#include "qma7981.h"
#include <math.h>

static const char *TAG = "IMU";

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;

static uint64_t max_imu_test_time = 10000000;
static float imu_lower_thresh = 0.25;

static void imu_test_task(void *arg)
{
    esp_err_t init_result = qma7981_init();
    if (init_result != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init imu. \n");
    }

    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        bool imu_pass;
        bool z_pass;
        bool x_pass;
        bool y_pass;
        xQueueReceive(queues_tests[TEST_IMU], &g_state_test, portMAX_DELAY);
        if (g_state_test == TEST_IMU)
        {
            ESP_LOGI(board_version, "--------------- Enter IMU Test ---------------\n");
            z_pass = false;
            x_pass = false;
            y_pass = false;
            bool timeout = false;
            float x, y, z = 0.0;

            ESP_LOGW(TAG, "Test Z axis\n");
            uint64_t start_time = esp_timer_get_time();
            while ((!timeout) && (!z_pass))
            {
                qma7981_get_acce(&x, &y, &z);
                z_pass = (fabs(x) < imu_lower_thresh) && (fabs(y) < imu_lower_thresh) && (z < (-1.0+imu_lower_thresh)) && (z > (-1.0-imu_lower_thresh));
                timeout = ((esp_timer_get_time() - start_time) > max_imu_test_time);
                // printf("timeout: %d, a: %f, x: %f, y: %f, z: %f\n", timeout, sqrt(x*x+y*y+z*z), x, y, z);
                vTaskDelay(10);
            }
            if (z_pass)
            {
                ESP_LOGW(TAG, "--------------- Z axis Test PASS ---------------\n");
            }
            else
            {
                ESP_LOGE(TAG, "--------------- Z axis Test FAIL ---------------\n");
            }

            ESP_LOGW(TAG, "Test X axis\n");
            start_time = esp_timer_get_time();
            timeout = false;
            while ((!timeout) && (!x_pass))
            {
                qma7981_get_acce(&x, &y, &z);
                x_pass = (fabs(y) < imu_lower_thresh) && (fabs(z) < imu_lower_thresh) && (x < (-1.0+imu_lower_thresh)) && (x > (-1.0-imu_lower_thresh));
                timeout = ((esp_timer_get_time() - start_time) > max_imu_test_time);
                // printf("timeout: %d, a: %f, x: %f, y: %f, z: %f\n", timeout, sqrt(x*x+y*y+z*z), x, y, z);
                vTaskDelay(10);
            }
            if (x_pass)
            {
                ESP_LOGW(TAG, "--------------- X axis Test PASS ---------------\n");
            }
            else
            {
                ESP_LOGE(TAG, "--------------- X axis Test FAIL ---------------\n");
            }

            ESP_LOGW(TAG, "Test Y axis\n");
            start_time = esp_timer_get_time();
            timeout = false;
            while ((!timeout) && (!y_pass))
            {
                qma7981_get_acce(&x, &y, &z);
                y_pass = (fabs(x) < imu_lower_thresh) && (fabs(z) < imu_lower_thresh) && (y < (-1.0+imu_lower_thresh)) && (y > (-1.0-imu_lower_thresh));
                timeout = ((esp_timer_get_time() - start_time) > max_imu_test_time);
                // printf("timeout: %d, a: %f, x: %f, y: %f, z: %f\n", timeout, sqrt(x*x+y*y+z*z), x, y, z);
                vTaskDelay(10);
            }
            if (y_pass)
            {
                ESP_LOGW(TAG, "--------------- Y axis Test PASS ---------------\n");
            }
            else
            {
                ESP_LOGE(TAG, "--------------- Y axis Test FAIL ---------------\n");
            }

            imu_pass = z_pass && x_pass && y_pass;
            if (imu_pass)
            {
                ESP_LOGI(TAG, "--------------- IMU Test PASS ---------------\n");
            }
            else
            {
                ESP_LOGE(TAG, "--------------- IMU Test FAIL ---------------\n");
            }

            xQueueSend(queue_test_result, &imu_pass, portMAX_DELAY);
        }
        else
        {
            ESP_LOGE(TAG, "--------------- Receive Test Code Error ---------------\n");
            bool result = false;
            xQueueSend(queue_test_result, &result, portMAX_DELAY);
        }
    }
}

void register_imu_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue)
{
    queue_test_result = result_queue;
    queues_tests = test_queues;

    xTaskCreatePinnedToCore(imu_test_task, "imu_test_task", 3 * 1024, NULL, 5, NULL, 1);
}