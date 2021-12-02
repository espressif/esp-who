#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "test_logic.h"

static const char *TAG = "SD_CARD";

#define MOUNT_POINT "/sdcard"

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;

esp_err_t sd_card_mount(const char *mount_point, sdmmc_card_t **card)
{
    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    // sdmmc_card_t *card;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // To use 1-line SD mode, change this to 1:
    slot_config.width = 1;

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
    slot_config.clk = GPIO_NUM_39;
    slot_config.cmd = GPIO_NUM_38;
    slot_config.d0 = GPIO_NUM_40;
    // slot_config.d1 = GPIO_NUM_4;
    // slot_config.d2 = GPIO_NUM_12;
    // slot_config.d3 = GPIO_NUM_13;

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
    }
    // else
    // {
    //     sdmmc_card_print_info(stdout, card);
    // }
    return ret;
}

int FatfsComboWrite(const void* buffer, int size, int count, FILE* stream)
{
    int res = 0;
    res = fwrite(buffer, size, count, stream);
    res |= fflush(stream);        // required by stdio, this will empty any buffers which newlib holds
    res |= fsync(fileno(stream)); // this will tell the filesystem driver to write data to disk

    return res;
}

static void sd_card_test_task(void *arg)
{
    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // To use 1-line SD mode, change this to 1:
    slot_config.width = 1;

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
    slot_config.clk = GPIO_NUM_39;
    slot_config.cmd = GPIO_NUM_38;
    slot_config.d0 = GPIO_NUM_40;
    // slot_config.d1 = GPIO_NUM_4;
    // slot_config.d2 = GPIO_NUM_12;
    // slot_config.d3 = GPIO_NUM_13;

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        xQueueReceive(queues_tests[TEST_SD_CARD], &g_state_test, portMAX_DELAY);
        bool sd_card_pass = false;
        if (g_state_test == TEST_SD_CARD)
        {
            ESP_LOGI(board_version, "--------------- Enter SD CARD Test ---------------\n");
            ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

            if (ret != ESP_OK)
            {
                if (ret == ESP_FAIL)
                {
                    ESP_LOGE(TAG, "Failed to mount filesystem. "
                                  "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                                  "Make sure SD card lines have pull-up resistors in place.",
                             esp_err_to_name(ret));
                }

                sd_card_pass = false;
                ESP_LOGE(TAG, "--------------- SD CARD Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
            }
            else
            {
                sdmmc_card_print_info(stdout, card);
                const char *file_hello = MOUNT_POINT "/hello.txt";

                // ESP_LOGI(TAG, "Opening file %s", file_hello);
                FILE *f = fopen(file_hello, "w");
                if (f == NULL)
                {
                    ESP_LOGE(TAG, "Failed to open file for writing");
                    sd_card_pass = false;
                    ESP_LOGE(TAG, "--------------- SD CARD Test FAIL ---------------\n");
                    xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
                    esp_vfs_fat_sdcard_unmount(mount_point, card);
                    continue;
                }

                char line_write[50];
                strcpy(line_write, "Hello ");
                strcat(line_write, card->cid.name);
                strcat(line_write, "!\n");

                fprintf(f, "Hello %s!\n", card->cid.name);
                fclose(f);
                // ESP_LOGI(TAG, "File written");
                const char *file_foo = MOUNT_POINT "/foo.txt";

                // Check if destination file exists before renaming
                struct stat st;
                if (stat(file_foo, &st) == 0)
                {
                    // Delete it if it exists
                    unlink(file_foo);
                }

                // Rename original file
                // ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
                if (rename(file_hello, file_foo) != 0)
                {
                    ESP_LOGE(TAG, "Rename failed");
                    sd_card_pass = false;
                    ESP_LOGE(TAG, "--------------- SD CARD Test FAIL ---------------\n");
                    xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
                    esp_vfs_fat_sdcard_unmount(mount_point, card);
                    continue;
                }

                // Open renamed file for reading
                // ESP_LOGI(TAG, "Reading file %s", file_foo);
                f = fopen(file_foo, "r");
                if (f == NULL)
                {
                    ESP_LOGE(TAG, "Failed to open file for reading");
                    sd_card_pass = false;
                    ESP_LOGE(TAG, "--------------- SD CARD Test FAIL ---------------\n");
                    xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
                    esp_vfs_fat_sdcard_unmount(mount_point, card);
                    continue;
                }

                // Read a line from file
                char line[64];
                fgets(line, sizeof(line), f);
                fclose(f);

                if (strcmp(line_write, line) == 0)
                {
                    sd_card_pass = true;
                    ESP_LOGI(TAG, "--------------- SD CARD Test PASS ---------------\n");
                    xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
                }
                else
                {
                    ESP_LOGE(TAG, "Wirte: '%s'", line_write);
                    ESP_LOGE(TAG, "Read: '%s'", line);
                    sd_card_pass = false;
                    ESP_LOGE(TAG, "--------------- SD CARD Test FAIL ---------------\n");
                    xQueueSend(queue_test_result, &sd_card_pass, portMAX_DELAY);
                }
                esp_vfs_fat_sdcard_unmount(mount_point, card);
            }
        }
        else
        {
            ESP_LOGE(TAG, "--------------- Receive Test Code Error ---------------\n");
            bool result = false;
            xQueueSend(queue_test_result, &result, portMAX_DELAY);
        }
    }
}

void register_sd_card_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue)
{
    queue_test_result = result_queue;
    queues_tests = test_queues;
    xTaskCreatePinnedToCore(sd_card_test_task, "sd_card_test_task", 3 * 1024, NULL, 5, NULL, 0);
}