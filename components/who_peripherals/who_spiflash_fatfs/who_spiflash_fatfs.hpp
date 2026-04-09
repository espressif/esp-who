#pragma once

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

esp_err_t spiflash_fatfs_mount();
esp_err_t spiflash_fatfs_unmount();
