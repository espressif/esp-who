#include "app_speech.hpp"

#include "driver/i2s.h"
#include "esp_log.h"

#include "dl_lib_coefgetter_if.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_spiffs.h"

#define I2S_CHANNEL_NUM 1
#define I2S_CH ((i2s_port_t)1)

static const char *TAG = "App/Speech";

static void spiffs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/srmodel",
        .partition_label = "model",
        .max_files = 5,
        .format_if_mount_failed = true};
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("model", &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

static void i2s_init(void)
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // the mode must be set according to DSP configuration
        .sample_rate = 16000,                                // must be the same as DSP configuration
        .bits_per_sample = (i2s_bits_per_sample_t)32,        // must be the same as DSP configuration
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,         // must be the same as DSP configuration
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
        .dma_buf_count = 3,
        .dma_buf_len = 300,
    };
    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = 41,                  // IIS_SCLK
        .ws_io_num = 42,                   // IIS_LCLK
        .data_out_num = I2S_PIN_NO_CHANGE, // IIS_DSIN
        .data_in_num = 2                   // IIS_DOUT
    };
    i2s_driver_install(I2S_CH, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_CH, &pin_config);
    i2s_zero_dma_buffer(I2S_CH);
}

static void feed_handler(AppSpeech *self)
{
    esp_afe_sr_data_t *afe_data = self->afe_data;
    int audio_chunksize = self->afe_handle->get_feed_chunksize(afe_data);
    int nch = self->afe_handle->get_channel_num(afe_data);
    size_t samp_len = audio_chunksize * I2S_CHANNEL_NUM;
    size_t samp_len_bytes = samp_len * sizeof(int32_t);
    int32_t *i2s_buff = (int32_t *)malloc(samp_len_bytes);
    assert(i2s_buff);
    size_t bytes_read;
    // FILE *fp = fopen("/sdcard/out", "a+");
    // if (fp == NULL) ESP_LOGE(TAG,"can not open file\n");

    while (true)
    {
        i2s_read(I2S_CH, i2s_buff, samp_len_bytes, &bytes_read, portMAX_DELAY);

        for (int i = 0; i < samp_len; ++i)
        {
            i2s_buff[i] = i2s_buff[i] >> 14; // 32:8为有效位， 8:0为低8位， 全为0， AFE的输入为16位语音数据，拿29：13位是为了对语音信号放大。
        }
        // FatfsComboWrite(i2s_buff, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), 1, fp);

        self->afe_handle->feed(afe_data, (int16_t *)i2s_buff);
    }
    self->afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

static void detect_hander(AppSpeech *self)
{
    esp_afe_sr_data_t *afe_data = self->afe_data;
    int afe_chunksize = self->afe_handle->get_fetch_chunksize(afe_data);
    int nch = self->afe_handle->get_channel_num(afe_data);
    int16_t *buff = (int16_t *)malloc(afe_chunksize * sizeof(int16_t));
    assert(buff);

    static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
    model_iface_data_t *model_data = multinet->create((const model_coeff_getter_t *)&MULTINET_COEFF, 5760);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data);
    assert(mu_chunksize == afe_chunksize);

    // FILE *fp = fopen("/sdcard/out", "w");
    // if (fp == NULL) ESP_LOGE(TAG,"can not open file\n");

    ESP_LOGI(TAG, "Ready");

    self->detected = false;

    while (true)
    {
        int res = self->afe_handle->fetch(afe_data, buff);

        if (res == AFE_FETCH_WWE_DETECTED)
        {
            ESP_LOGI(TAG, ">>> Say your command <<<");
            self->detected = true;
            self->afe_handle->disable_wakenet(afe_data);
            self->notify();
        }

        if (self->detected)
        {
            // Detect command
            self->command = (command_word_t)multinet->detect(model_data, buff);
            // FatfsComboWrite(buff, afe_chunksize * sizeof(int16_t), 1, fp);

            if (self->command == COMMAND_NOT_DETECTED)
                continue;
            else if (self->command == COMMAND_TIMEOUT)
            {
                self->afe_handle->enable_wakenet(afe_data);
                // self->afe_handle->enable_aec(afe_data);

                self->detected = false;
                ESP_LOGI(TAG, ">>> Waiting to be waken up <<<");
                self->notify();
            }
            else
            {
                self->notify();
                ESP_LOGD(TAG, "Command: %d", self->command);

#ifndef CONFIG_SR_MN_CN_MULTINET3_CONTINUOUS_RECOGNITION
                self->afe_handle->enable_wakenet(afe_data);
                // self->afe_handle->enable_aec(afe_data);
                self->detected = false;
                ESP_LOGI(TAG, ">>> Waiting to be waken up <<<");
#endif
                self->command = COMMAND_TIMEOUT;
                self->notify();
            }
        }
    }
    self->afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

AppSpeech::AppSpeech() : afe_handle(&esp_afe_sr_1mic), detected(false), command(COMMAND_TIMEOUT)
{
    spiffs_init();
    i2s_init();
    // sd_card_mount("/sdcard");
    afe_config_t afe_config = {
        .aec_init = true,
        .se_init = true,
        .vad_init = true,
        .wakenet_init = true,
        .vad_mode = 3,
        .wakenet_model = &WAKENET_MODEL,
        .wakenet_coeff = (const model_coeff_getter_t *)&WAKENET_COEFF,
        .wakenet_mode = DET_MODE_2CH_90,
        .afe_mode = SR_MODE_LOW_COST,
        .afe_perferred_core = 0,
        .afe_perferred_priority = 5,
        .afe_ringbuf_size = 50,
        .alloc_from_psram = AFE_PSRAM_MEDIA_COST,
        .agc_mode = 2,
    };
    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.vad_init = false;
    afe_config.afe_ringbuf_size = 10;
    this->afe_data = this->afe_handle->create_from_config(&afe_config);
}

void AppSpeech::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)feed_handler, "App/SR/Feed", 4 * 1024, this, 5, NULL, 0);
    xTaskCreatePinnedToCore((TaskFunction_t)detect_hander, "App/SR/Detect", 5 * 1024, this, 5, NULL, 0);
}