#include "who_speech_recognition.h"
#include "esp_log.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "driver/i2s.h"
#include "esp_spiffs.h"
#include "who_led.h"

#define I2S_CHANNEL_NUM 1
#define I2S_CH 1

static QueueHandle_t queue_result = NULL;
static QueueHandle_t queue_control = NULL;

int detect_flag = 0;
static esp_afe_sr_iface_t *afe_handle = NULL;

static void spiffs_init(void)
{
    printf("Initializing SPIFFS\n");
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
            printf("Failed to mount or format filesystem\n");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            printf("Failed to find SPIFFS partition\n");
        }
        else
        {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("model", &total, &used);
    if (ret != ESP_OK)
    {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    }
    else
    {
        printf("Partition size: total: %d, used: %d\n", total, used);
    }
}

static void i2s_test_init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,       //the mode must be set according to DSP configuration
        .sample_rate = 16000,                        //must be the same as DSP configuration
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //must be the same as DSP configuration
        .bits_per_sample = 32,                       //must be the same as DSP configuration
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 3,
        .dma_buf_len = 300,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
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

void feed_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    size_t samp_len = audio_chunksize * I2S_CHANNEL_NUM;
    size_t samp_len_bytes = samp_len * sizeof(int32_t);
    int32_t *i2s_buff = (int32_t *)malloc(samp_len_bytes);
    assert(i2s_buff);
    size_t bytes_read;
    // FILE *fp = fopen("/sdcard/out", "a+");
    // if (fp == NULL) printf("can not open file\n");

    while (1)
    {
        i2s_read(I2S_CH, i2s_buff, samp_len_bytes, &bytes_read, portMAX_DELAY);

        for (int i = 0; i < samp_len; ++i)
        {
            i2s_buff[i] = i2s_buff[i] >> 13;    // 32:8为有效位， 8:0为低8位， 全为0， AFE的输入为16位语音数据，拿29：13位是为了对语音信号放大。
        }
        // FatfsComboWrite(i2s_buff, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), 1, fp);

        afe_handle->feed(afe_data, (int16_t *)i2s_buff);
    }
    afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

void detect_Task(void *arg)
{
    int control_id = 0;
    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    int16_t *buff = malloc(afe_chunksize * sizeof(int16_t));
    assert(buff);
    static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
    model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF, 5760);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data);
    assert(mu_chunksize == afe_chunksize);
    printf("------------detect start------------\n");
    // FILE *fp = fopen("/sdcard/out", "w");
    // if (fp == NULL) printf("can not open file\n");
    while (1)
    {
        int res = afe_handle->fetch(afe_data, buff);

        if (res == AFE_FETCH_WWE_DETECTED)
        {
            printf("wakeword detected\n");
            printf("-----------LISTENING-----------\n");
            detect_flag = 1;
            afe_handle->disable_wakenet(afe_data);
        }
        // if (res == AFE_FETCH_CHANNEL_VERIFIED) {
        //     play_voice = -1;
        // }

        if (detect_flag == 1)
        {
            if (queue_control)
            {
                control_id = LED_ALWAYS_ON;
                xQueueSend(queue_control, &control_id, portMAX_DELAY);
            }

            int command_id = multinet->detect(model_data, buff);
            // FatfsComboWrite(buff, afe_chunksize * sizeof(int16_t), 1, fp);

            if (command_id >= -2)
            {
                if (command_id > -1)
                {
                    if (queue_control)
                    {
                        control_id = LED_BLINK_1S;
                        xQueueSend(queue_control, &control_id, portMAX_DELAY);
                    }
                    // play_voice = command_id;
                    printf("command_id: %d\n", command_id);
                    xQueueSend(queue_result, &command_id, portMAX_DELAY);
#ifndef CONFIG_SR_MN_CN_MULTINET3_CONTINUOUS_RECOGNITION
                    afe_handle->enable_wakenet(afe_data);
                    // afe_handle->enable_aec(afe_data);
                    detect_flag = 0;
                    printf("\n-----------awaits to be waken up-----------\n");
#endif
                }

                if (command_id == -2)
                {
                    if (queue_control)
                    {
                        control_id = LED_ALWAYS_OFF;
                        xQueueSend(queue_control, &control_id, portMAX_DELAY);
                    }
                    afe_handle->enable_wakenet(afe_data);
                    // afe_handle->enable_aec(afe_data);
                    detect_flag = 0;
                    printf("\n-----------awaits to be waken up-----------\n");
                }
            }
        }
    }
    afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

void register_speech_recognition(QueueHandle_t result_o, QueueHandle_t control_o)
{
    queue_result = result_o;
    queue_control = control_o;

    spiffs_init();
    i2s_test_init();
    // sd_card_mount("/sdcard");
    afe_handle = &esp_afe_sr_1mic;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.vad_init = false;
    // afe_config.afe_ringbuf_size = 10;
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    xTaskCreatePinnedToCore(&feed_Task, "sr_feed", 4 * 1024, (void *)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&detect_Task, "sr_detect", 4 * 1024, (void *)afe_data, 5, NULL, 0);
}