#include "test_logic.h"

#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "driver/i2s.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#define I2S_CHANNEL_NUM 1
#define I2S_CH 1
#define RECORD_AUDIO 1

static esp_afe_sr_iface_t *afe_handle = NULL;
static SemaphoreHandle_t semaphore_test;
static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t queue_test_start = NULL;
static QueueHandle_t *queues_tests = NULL;
static const char *TAG = "MIC";

static void spiffs_init(void)
{
#include "esp_spiffs.h"
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
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,        //the mode must be set according to DSP configuration
        .sample_rate = 16000,                         //must be the same as DSP configuration
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //must be the same as DSP configuration
        .bits_per_sample = 32,                        //must be the same as DSP configuration
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

static inline void mic_data_trans(int32_t *i2s_buf, int data_len)
{
    // int16_t *temp = (int16_t *)i2s_buf;
    for (int i = 0; i < data_len; ++i)
    {
        i2s_buf[i] = i2s_buf[i] >> 14;
    }
}

static inline void mic_data_trans_old(int32_t *i2s_buf, int data_len)
{
    // int16_t *temp = (int16_t *)i2s_buf;
    int loop_len = data_len / 4;
    for (int i = 0; i < loop_len; ++i)
    {
        int s1 = ((i2s_buf[i * 4] + i2s_buf[i * 4 + 1]) >> 13) & 0x0000FFFF;
        int s2 = ((i2s_buf[i * 4 + 2] + i2s_buf[i * 4 + 3]) << 3) & 0xFFFF0000;
        i2s_buf[i] = s1 | s2;
    }
}

static void mic_record_test(void *arg)
{
    i2s_test_init();

    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    // int nch = afe_handle->get_channel_num(afe_data);
    size_t samp_len = audio_chunksize * I2S_CHANNEL_NUM;
    size_t samp_len_bytes = samp_len * sizeof(int32_t);
    int32_t *i2s_buff = malloc(samp_len_bytes);
    assert(i2s_buff);
    size_t bytes_read;

    sdmmc_card_t *card = NULL;
    sd_card_mount("/sdcard", &card);

    FILE *fp = fopen("/sdcard/i2s_16b_double_hilexin", "w");
    if (fp == NULL)
        printf("can not open file\n");
    else
        printf("start recording\n");

    while (1)
    {
        i2s_read(I2S_CH, i2s_buff, samp_len_bytes, &bytes_read, portMAX_DELAY);

        mic_data_trans(i2s_buff, samp_len);

        int ret = FatfsComboWrite(i2s_buff, samp_len_bytes, 1, fp);
    }
    fclose(fp);
    esp_vfs_fat_sdcard_unmount("/sdcard", card);
    afe_handle->destroy(afe_data);

    vTaskDelete(NULL);
}

static bool mic_pass = false;
static bool time_out = false;
static uint64_t max_mic_test_time = 15000000;

static void i2s_test_task(void *arg)
{
    i2s_test_init();

    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    // int nch = afe_handle->get_channel_num(afe_data);
    size_t samp_len = audio_chunksize * I2S_CHANNEL_NUM;
    size_t samp_len_bytes = samp_len * sizeof(int32_t);
    int32_t *i2s_buff = malloc(samp_len_bytes);
    assert(i2s_buff);

    // void *test_mem = heap_caps_malloc(32*1024, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);;
    // assert(test_mem != NULL);

    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        xQueueReceive(queues_tests[TEST_MIC], &g_state_test, portMAX_DELAY);
        if (g_state_test == TEST_MIC)
        {
            ESP_LOGI("ESP32-S3-EYE", "--------------- Enter MIC Test ---------------\n");
            ESP_LOGW(TAG, "Please Say \"Hi, Lexin\"\n");
#if RECORD_AUDIO
            sdmmc_card_t *card = NULL;
            sd_card_mount("/sdcard", &card);
            FILE *fp = fopen("/sdcard/i2s_test", "w");
            if (fp == NULL)
                printf("can not open file: \"/sdcard/i2s_test\"\n");
#endif
            xSemaphoreTake(semaphore_test, portMAX_DELAY);
            mic_pass = false;
            time_out = false;
            xSemaphoreGive(semaphore_test);

            bool detect_start = true;
            xQueueSend(queue_test_start, &detect_start, portMAX_DELAY);
            size_t bytes_read;

            uint64_t start_time = esp_timer_get_time();
            while (1)
            {
                i2s_read(I2S_CH, i2s_buff, samp_len_bytes, &bytes_read, portMAX_DELAY);
                mic_data_trans(i2s_buff, samp_len);
#if RECORD_AUDIO
                int ret = FatfsComboWrite(i2s_buff, samp_len_bytes, 1, fp);
#endif
                afe_handle->feed(afe_data, (int16_t *)i2s_buff);
                int64_t end_time = esp_timer_get_time();
                xSemaphoreTake(semaphore_test, portMAX_DELAY);
                time_out = ((end_time - start_time) > max_mic_test_time);
                bool stop_flag = (mic_pass || time_out);
                xSemaphoreGive(semaphore_test);
                if (stop_flag)
                {
                    break;
                }
            }
            xSemaphoreTake(semaphore_test, portMAX_DELAY);
            if (mic_pass)
            {
                ESP_LOGI(TAG, "--------------- MIC Test PASS ---------------\n");
            }
            else
            {
                ESP_LOGE(TAG, "--------------- MIC Test FAIL ---------------\n");
            }
            xQueueSend(queue_test_result, &mic_pass, portMAX_DELAY);
            xSemaphoreGive(semaphore_test);
#if RECORD_AUDIO
            fclose(fp);
            esp_vfs_fat_sdcard_unmount("/sdcard", card);
#endif
        }
    }
    afe_handle->destroy(afe_data);

    vTaskDelete(NULL);
}

static void detect_test_task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    // int nch = afe_handle->get_channel_num(afe_data);
    int16_t *buff = malloc(afe_chunksize * sizeof(int16_t));
    assert(buff);

    while (1)
    {
        bool detect_flag;
        xQueueReceive(queue_test_start, &detect_flag, portMAX_DELAY);
        while (1)
        {
            int res = afe_handle->fetch(afe_data, buff);

            if (res == AFE_FETCH_WWE_DETECTED)
            {
                xSemaphoreTake(semaphore_test, portMAX_DELAY);
                mic_pass = true;
                xSemaphoreGive(semaphore_test);
                break;
            }

            xSemaphoreTake(semaphore_test, portMAX_DELAY);
            bool time_out_detect = time_out;
            xSemaphoreGive(semaphore_test);
            if (time_out_detect)
            {
                break;
            }
        }
    }

    afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

void app_mic_test()
{
    spiffs_init();
    afe_handle = &esp_afe_sr_1mic;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.vad_init = false;
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    xTaskCreatePinnedToCore(&mic_record_test, "i2s", 6 * 1024, (void *)afe_data, 15, NULL, 0);
}

void register_mic_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue)
{
    queue_test_result = result_queue;
    queues_tests = test_queues;
    queue_test_start = xQueueCreate(1, sizeof(bool));
    semaphore_test = xSemaphoreCreateMutex();

    spiffs_init();
    afe_handle = &esp_afe_sr_1mic;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.vad_init = false;
    // afe_config.afe_ringbuf_size = 10;
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    xTaskCreatePinnedToCore(&i2s_test_task, "i2s_test_task", 6 * 1024, (void *)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&detect_test_task, "detect_test_task", 6 * 1024, (void *)afe_data, 5, NULL, 1);
}
