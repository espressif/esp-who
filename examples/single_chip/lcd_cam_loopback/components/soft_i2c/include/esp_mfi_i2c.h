// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_I2C_SOFTWARE    1

#define CONFIG_I2C_RETRY_COUNT 0

#define I2C_MASTER_SDA_GPIO    8
#define I2C_MASTER_SCL_GPIO    7

/**
 * @brief Initialize I2C information
 */
int esp_mfi_i2c_init(void);

/**
 * @brief Finish I2C information
 */
int esp_mfi_i2c_end(void);

/**
 * @brief write data buffer to slave
 */
int esp_mfi_i2c_write(uint8_t slvaddr, uint8_t regaddr, uint8_t *buff, uint32_t len);

/**
 * @brief read data form slave
 */
int esp_mfi_i2c_read(uint8_t slvaddr, uint8_t regaddr, uint8_t *buff, uint32_t len);

#ifdef __cplusplus
}
#endif
