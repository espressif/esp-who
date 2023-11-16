/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_adc_cal.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "who_adc_button.h"

//ADC Channels
#define ADC1_EXAMPLE_CHAN0 ADC_CHANNEL_0
//ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11
//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define PRESS_INTERVAL 500000

static uint32_t voltage = 0;

static const char *TAG = "ADC SINGLE";

// static esp_adc_cal_characteristics_t adc1_chars;

static button_adc_config_t buttons[4] = {{1, 2800, 3000}, {2, 2250, 2450}, {3, 300, 500}, {4, 850, 1050}};

button_adc_config_t *adc_buttons;
int adc_button_num;
static QueueHandle_t xQueueKeyStateO = NULL;

    
static bool adc_calibration_init(adc_cali_handle_t *out_handle)
{
    esp_err_t ret = ESP_FAIL;
    adc_cali_handle_t handle = NULL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_EXAMPLE_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_EXAMPLE_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
#endif

    *out_handle = handle;
    if (ret == ESP_OK) 
    {
        calibrated = true;
        ESP_LOGI(TAG, "Calibration Success");
    } 
    else if (ret == ESP_ERR_NOT_SUPPORTED) 
    {
        ESP_LOGW(TAG, "calibration fail due to lack of eFuse bits");
    } 
    else 
    {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return calibrated;
}

static void adc_button_task(void *arg)
{
    int last_button_pressed = -1;
    int button_pressed = -1;
    int64_t backup_time = esp_timer_get_time();
    int64_t last_time = esp_timer_get_time();

    //ADC1 config
    // ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    // ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        // .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_EXAMPLE_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_EXAMPLE_CHAN0, &config));

    adc_cali_handle_t adc1_cali_handle;
    adc_calibration_init(&adc1_cali_handle);

    while (1)
    {
        adc_oneshot_read(adc1_handle, ADC1_EXAMPLE_CHAN0, &voltage);
        // ESP_LOGI(TAG, "raw  data: %d", voltage);

        // int adc_raw = 0;
        // adc_oneshot_read(adc1_handle, ADC1_EXAMPLE_CHAN0, &adc_raw);
        // adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage);
        // ESP_LOGI(TAG, "Raw: %"PRIu32"\tVoltage: %dmV", adc_raw, voltage);

        backup_time = esp_timer_get_time();
        for (int i = 0; i < adc_button_num; ++i)
        {
            if ((voltage >= adc_buttons[i].min) && (voltage <= adc_buttons[i].max))
            {
                button_pressed = adc_buttons[i].button_index;
                printf("button_pressed: %d\n", button_pressed);
                if ((button_pressed != last_button_pressed) || ((backup_time - last_time) > PRESS_INTERVAL))
                {
                    last_button_pressed = button_pressed;
                    last_time = backup_time;
                    xQueueOverwrite(xQueueKeyStateO, &button_pressed);
                    break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void register_adc_button(button_adc_config_t *buttons_ptr, int button_num, const QueueHandle_t key_state_o)
{
    xQueueKeyStateO = key_state_o;
    if (buttons_ptr == NULL)
    {
        adc_buttons = buttons;
        adc_button_num = 4;
    }
    else
    {
        adc_buttons = buttons_ptr;
        adc_button_num = button_num;
    }
    xTaskCreatePinnedToCore(adc_button_task, "adc_button_scan_task", 3 * 1024, NULL, 5, NULL, 0);
}