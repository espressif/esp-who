/**
 * @file qma7981.h
 * @brief 
 * @version 0.1
 * @date 2021-09-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	QMA_RANGE_2G = 0b0001,
	QMA_RANGE_4G = 0b0010,
	QMA_RANGE_8G = 0b0100,
	QMA_RANGE_16G = 0b1000,
	QMA_RANGE_32G = 0b1111,
} qma_range_t;	/* Others will be 2G */

typedef enum {
	QMA_BANDWIDTH_128_HZ = 0b111,
	QMA_BANDWIDTH_256_HZ = 0b110,
	QMA_BANDWIDTH_1024_HZ = 0b101,
} qma_bandwidth_t;

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t qma7981_init(void);

/**
 * @brief 
 * 
 * @param range 
 * @return esp_err_t 
 */
esp_err_t qma7981_set_range(qma_range_t range);

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return esp_err_t 
 */
esp_err_t qma7981_get_acce(float *x, float *y, float *z);

#ifdef __cplusplus
}
#endif
