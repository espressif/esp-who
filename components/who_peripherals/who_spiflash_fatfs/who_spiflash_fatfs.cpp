#include "who_spiflash_fatfs.hpp"

static const char *TAG = "spiflash_fatfs";
static wl_handle_t wl_handle;

esp_err_t spiflash_fatfs_mount()
{
    esp_vfs_fat_mount_config_t mount_config = {};
    mount_config.max_files = 5;
    mount_config.format_if_mount_failed = true;
    ESP_RETURN_ON_ERROR(esp_vfs_fat_spiflash_mount_rw_wl(
        CONFIG_SPIFLASH_MOUNT_POINT, CONFIG_SPIFLASH_MOUNT_PARTITION, &mount_config, &wl_handle), TAG, "Failed to mount spiflash fatfs.");
    return ESP_OK;
}

esp_err_t spiflash_fatfs_unmount()
{
    ESP_RETURN_ON_ERROR(
        esp_vfs_fat_spiflash_unmount_rw_wl(CONFIG_SPIFLASH_MOUNT_POINT, wl_handle), TAG, "Failed to unmount spiflash fatfs.");
    return ESP_OK;
}
