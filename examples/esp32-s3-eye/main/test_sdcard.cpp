#include "test_sdcard.hpp"

#include <string.h>

#include <sys/unistd.h>
#include <sys/stat.h>

#include "esp_vfs_fat.h"
#include "test_logic.h"
#include "define.h"

static const char *TAG = "test/sd";

#define MOUNT_POINT "/sdcard"

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
    else
    {
        sdmmc_card_print_info(stdout, *card);
    }
    return ret;
}

int FatfsComboWrite(const void *buffer, int size, int count, FILE *stream)
{
    int res = 0;
    res = fwrite(buffer, size, count, stream);
    res |= fflush(stream);        // required by stdio, this will empty any buffers which newlib holds
    res |= fsync(fileno(stream)); // this will tell the filesystem driver to write data to disk

    return res;
}

bool test_sdcard()
{
    static bool pass = false;

    if (!pass)
    {
        printf(FORMAT_MENU("SDCard Test"));

        sdmmc_card_t *card;

        // Options for mounting the filesystem.
        // If format_if_mount_failed is set to true, SD card will be partitioned and
        // formatted in case when mounting fails.
        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024};
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

        bool status = true;
        bool mounted = true;

        do
        {
            // Mount SDCard
            esp_err_t ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
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

                status = false;
                mounted = false;
                break;
            }
            sdmmc_card_print_info(stdout, card);

            // Open file
            char file_hello[] = MOUNT_POINT "/hello.txt";
            FILE *f = fopen(file_hello, "w");
            if (f == NULL)
            {
                ESP_LOGE(TAG, "Failed to open %s", file_hello);
                status = false;
                break;
            }
            ESP_LOGI(TAG, "Open %s", file_hello);

            // Write hello.txt
            char line_write[50];
            sprintf(line_write, "Hello %s!\n", card->cid.name);
            fprintf(f, line_write);
            fclose(f);
            ESP_LOGI(TAG, "Write \"Hello %s!\" to %s", card->cid.name, file_hello);

            // Delete foo.txt if it exists
            char file_foo[] = MOUNT_POINT "/foo.txt";
            struct stat st;
            if (stat(file_foo, &st) == 0)
            {
                unlink(file_foo); // Delete it if it exists
            }
            ESP_LOGI(TAG, "Delete %s if it exists", file_foo);

            // Rename hello.txt to foo.txt
            if (rename(file_hello, file_foo) != 0)
            {
                ESP_LOGE(TAG, "Fail to rename %s to %s", file_hello, file_foo);
                status = false;
                break;
            }
            ESP_LOGI(TAG, "Rename %s to %s", file_hello, file_foo);

            // Open foo.txt
            f = fopen(file_foo, "r");
            if (f == NULL)
            {
                ESP_LOGE(TAG, "Failed to open %s", file_foo);
                status = false;
                break;
            }
            ESP_LOGI(TAG, "Open %s", file_foo);

            // Read a line from file
            char line_read[64];
            fgets(line_read, sizeof(line_read), f);
            fclose(f);

            if (strcmp(line_write, line_read) != 0)
            {
                ESP_LOGE(TAG, "Wirte: \"%s\"", line_write);
                ESP_LOGE(TAG, "Read: \"%s\"", line_read);
                status = false;
            }
            ESP_LOGI(TAG, "Context in %s is correct", file_foo);

        } while (0);

        if (mounted)
            esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);

        printf(status ? FORMAT_PASS : FORMAT_FAIL);

        pass = status;
    }

    return pass;
}
