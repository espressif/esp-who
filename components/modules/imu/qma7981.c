/**
 * @file qma7981.c
 * @brief 
 * @version 0.1
 * @date 2021-09-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// #include "bsp_i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "i2c_bus.h"
#include "qma7981.h"

#define QMA7981_REG_CHIP_ID 0x00
#define QMA7981_REG_DX_L 0x01
#define QMA7981_REG_DX_H 0x02
#define QMA7981_REG_DY_L 0x03
#define QMA7981_REG_DY_H 0x04
#define QMA7981_REG_DZ_L 0x05
#define QMA7981_REG_DZ_H 0x06
#define QMA7981_REG_STEP_L 0x07
#define QMA7981_REG_STEP_H 0x08
#define QMA7981_REG_INT_STAT_0 0x0A
#define QMA7981_REG_INT_STAT_1 0x0B
#define QMA7981_REG_INT_STAT_4 0x0D
#define QMA7981_REG_RANGE 0x0F
#define QMA7981_REG_BAND_WIDTH 0x10
#define QMA7981_REG_PWR_MANAGE 0x11
#define QMA7981_REG_STEP_CONF_0 0x12
#define QMA7981_REG_STEP_CONF_1 0x13
#define QMA7981_REG_STEP_CONF_2 0x14
#define QMA7981_REG_STEP_CONF_3 0x15
#define QMA7981_REG_INT_EN_0 0x16
#define QMA7981_REG_INT_EN_1 0x17
#define QMA7981_REG_INT_MAP_0 0x19
#define QMA7981_REG_INT_MAP_1 0x1A
#define QMA7981_REG_INT_MAP_2 0x1B
#define QMA7981_REG_INT_MAP_3 0x1C
#define QMA7981_REG_SIG_STEP_TH 0x1D
#define QMA7981_REG_STEP 0x1F

static const char *TAG = "qma7981";
static qma_range_t qma_range = QMA_RANGE_2G;
static i2c_bus_device_handle_t qma7981_handle;
static i2c_bus_handle_t i2c_bus_handle;

/**
 * @brief 
 * 
 * @param reg_addr 
 * @param data 
 * @return esp_err_t 
 */
static esp_err_t qma7981_read_byte(uint8_t reg_addr, uint8_t *data)
{
	return i2c_bus_read_byte(qma7981_handle, reg_addr, data);
}

static esp_err_t qma7981_write_byte(uint8_t reg_addr, uint8_t data)
{
	return i2c_bus_write_byte(qma7981_handle, reg_addr, data);
}

static esp_err_t qma7981_read_bytes(uint8_t reg_addr, size_t data_len, uint8_t *data)
{
	return i2c_bus_read_bytes(qma7981_handle, reg_addr, data_len, data);
}

esp_err_t qma7981_init(void)
{
	if (NULL != qma7981_handle)
	{
		return ESP_FAIL;
	}

	esp_err_t ret_val = ESP_OK;

	// ret_val |= bsp_i2c_probe_addr(0x12);
	// if (ESP_OK == ret_val) {
	// 	ESP_LOGW(TAG, "QMA7981 detected!");
	// } else {
	// 	ESP_LOGE(TAG, "QMA7981 not detected!");
	// 	return ret_val;
	// }

	// ret_val |= bsp_i2c_add_device(&qma7981_handle, 0x12);

	// if (NULL == qma7981_handle) {
	//     return ESP_FAIL;
	// }
	uint32_t clk_speed = 400000;

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.scl_io_num = 5,
		.sda_io_num = 4,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = clk_speed,
	};

	i2c_bus_handle = i2c_bus_create(1, &conf);
	assert(i2c_bus_handle != NULL);

	qma7981_handle = i2c_bus_device_create(i2c_bus_handle, 0x12, clk_speed);
	assert(qma7981_handle != NULL);

	uint8_t id = 0;
	ret_val |= qma7981_read_byte(0x00, &id);
	ESP_LOGW(TAG, "ID : %02X", id);

	/* ******************************** ******************************** */
	ret_val |= qma7981_write_byte(QMA7981_REG_PWR_MANAGE, 0xC0); /* Exit sleep mode*/
	vTaskDelay(pdMS_TO_TICKS(20));
	ret_val |= qma7981_write_byte(QMA7981_REG_RANGE, QMA_RANGE_2G);				  /* Set range */
	ret_val |= qma7981_write_byte(QMA7981_REG_BAND_WIDTH, QMA_BANDWIDTH_1024_HZ); /* Set bandwidth */
	/* ******************************** ******************************** */

	return ret_val;
}

esp_err_t qma7981_get_step(uint16_t *data)
{
	esp_err_t ret_val = ESP_OK;
	uint8_t step_h = 0, step_l = 0;

	if (NULL == data)
	{
		return ESP_ERR_INVALID_ARG;
	}

	ret_val |= qma7981_read_byte(0x07, &step_l);
	ret_val |= qma7981_read_byte(0x08, &step_h);

	*data = (step_h << 8) + step_l;

	return ret_val;
}

/**
 * @brief 
 * 
 * @param range 
 * @return esp_err_t 
 */
esp_err_t qma7981_set_range(qma_range_t range)
{
	esp_err_t ret_val = qma7981_write_byte(QMA7981_REG_RANGE, range);
	qma_range = range;

	return ret_val;
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return esp_err_t 
 */
esp_err_t qma7981_get_acce(float *x, float *y, float *z)
{
	float multiple = 2;
	esp_err_t ret_val = ESP_OK;
	struct qma_acce_data_t
	{
		int16_t x;
		int16_t y;
		int16_t z;
	} data;

	switch (qma_range)
	{
	case QMA_RANGE_2G:
		multiple = 2;
		break;
	case QMA_RANGE_4G:
		multiple = 4;
		break;
	case QMA_RANGE_8G:
		multiple = 8;
		break;
	case QMA_RANGE_16G:
		multiple = 16;
		break;
	case QMA_RANGE_32G:
		multiple = 32;
		break;
	default:
		multiple = 2;
		break;
	}

	ret_val |= qma7981_read_bytes(QMA7981_REG_DX_L, 6, &data);

	/* QMA7981's range is 14 bit. Adjust data format */
	data.x >>= 2;
	data.y >>= 2;
	data.z >>= 2;

	/* Convert to acceleration of gravity */
	*x = data.x / (float)(1 << 13) * multiple;
	*y = data.y / (float)(1 << 13) * multiple;
	*z = data.z / (float)(1 << 13) * multiple;

	return ret_val;
}
