/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * OV2640 driver.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sccb.h"
#include "ov2640.h"
#include "ov2640_regs.h"
#include "ov2640_settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#else
#include "esp_log.h"
#endif

static void write_regs(sensor_t *sensor, const uint8_t (*regs)[2]){
    int i=0;
    while (regs[i][0]) {
        SCCB_Write(sensor->slv_addr, regs[i][0], regs[i][1]);
        i++;
    }
}

static int write_reg(sensor_t *sensor, uint8_t bank, uint8_t reg, uint8_t value)
{
    int ret=0;

    /* Switch to the given register bank */
    ret |= SCCB_Write(sensor->slv_addr, BANK_SEL, bank);

    /* Write the register value */
    ret |= SCCB_Write(sensor->slv_addr, reg, value);

    return ret;
}

static int write_reg_bits(sensor_t *sensor, uint8_t bank, uint8_t reg, uint8_t mask, int enable)
{
    int ret=0;
    uint8_t old;

    /* Switch to SENSOR register bank */
    ret |= SCCB_Write(sensor->slv_addr, BANK_SEL, bank);

    /* Update REG04 */
    old = SCCB_Read(sensor->slv_addr, reg);

    if (enable) {
        old |= mask;
    } else {
        old &= ~mask;
    }

    ret |= SCCB_Write(sensor->slv_addr, reg, old);
    return ret;
}

static int write_level_regs(sensor_t *sensor, const uint8_t** regs, uint8_t num_levels, int level)
{
    int ret=0;

    level += (num_levels / 2 + 1);
    if (level < 0 || level > num_levels) {
        return -1;
    }

    /* Switch to DSP register bank */
    ret |= SCCB_Write(sensor->slv_addr, BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    for (int i=0; i<sizeof(regs[0])/sizeof(regs[0][0]); i++) {
        ret |= SCCB_Write(sensor->slv_addr, regs[0][i], regs[level][i]);
    }

    return ret;
}

static int reset(sensor_t *sensor)
{
    /* Reset all registers */
    SCCB_Write(sensor->slv_addr, BANK_SEL, BANK_SEL_SENSOR);
    SCCB_Write(sensor->slv_addr, COM7, COM7_SRST);

    /* delay n ms */
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Write initial registers */
    write_regs(sensor, ov2640_settings_cif);

    return 0;
}

static int set_framesize(sensor_t *sensor, framesize_t framesize)
{
    int ret=0;
    uint16_t w = resolution[framesize][0];
    uint16_t h = resolution[framesize][1];
    const uint8_t (*regs)[2];

    if (framesize <= FRAMESIZE_CIF) {
        regs = ov2640_settings_to_cif;
    } else if (framesize <= FRAMESIZE_SVGA) {
        regs = ov2640_settings_to_svga;
    } else {
        regs = ov2640_settings_to_uxga;
    }
    
    /* Disable DSP */

    //ret |= SCCB_Write(sensor->slv_addr, BANK_SEL, BANK_SEL_DSP);
    //ret |= SCCB_Write(sensor->slv_addr, R_BYPASS, R_BYPASS_DSP_BYPAS);

    /* Write DSP input regsiters */
    write_regs(sensor, regs);

    /* Write output width */
    ret |= SCCB_Write(sensor->slv_addr, ZMOW, (w>>2)&0xFF); // OUTW[7:0] (real/4)
    ret |= SCCB_Write(sensor->slv_addr, ZMOH, (h>>2)&0xFF); // OUTH[7:0] (real/4)
    ret |= SCCB_Write(sensor->slv_addr, ZMHH, ((h>>8)&0x04)|((w>>10)&0x03)); // OUTH[8]/OUTW[9:8]

    /* Reset DSP */
    ret |= SCCB_Write(sensor->slv_addr, RESET, 0x00);

    /* Enable DSP */
    //ret |= SCCB_Write(sensor->slv_addr, BANK_SEL, BANK_SEL_DSP);
    //ret |= SCCB_Write(sensor->slv_addr, R_BYPASS, R_BYPASS_DSP_EN);
    /* delay n ms */
    vTaskDelay(10 / portTICK_PERIOD_MS);

    return ret;
}

static int set_pixformat(sensor_t *sensor, pixformat_t pixformat)
{
    /* read pixel format reg */
    switch (pixformat) {
        case PIXFORMAT_RGB565:
            write_regs(sensor, ov2640_settings_rgb565);
            break;
        case PIXFORMAT_YUV422:
        case PIXFORMAT_GRAYSCALE:
            write_regs(sensor, ov2640_settings_yuv422);
            break;
        case PIXFORMAT_JPEG:
            write_regs(sensor, ov2640_settings_jpeg3);
            break;
        default:
            return -1;
    }

    /* delay n ms */
    vTaskDelay(10 / portTICK_PERIOD_MS);

    return 0;
}

static int set_contrast(sensor_t *sensor, int level)
{
    return write_level_regs(sensor, (const uint8_t **)contrast_regs, NUM_CONTRAST_LEVELS, level);
}

static int set_brightness(sensor_t *sensor, int level)
{
    return write_level_regs(sensor, (const uint8_t **)brightness_regs, NUM_BRIGHTNESS_LEVELS, level);
}

static int set_saturation(sensor_t *sensor, int level)
{
    return write_level_regs(sensor, (const uint8_t **)saturation_regs, NUM_SATURATION_LEVELS, level);
}

static int set_gainceiling(sensor_t *sensor, gainceiling_t gainceiling)
{
    return write_reg(sensor, BANK_SEL_SENSOR, COM9, COM9_AGC_SET(gainceiling));
}

static int set_quality(sensor_t *sensor, int qs)
{
    return write_reg(sensor, BANK_SEL_DSP, QS, qs);
}

static int set_colorbar(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_SENSOR, COM7, COM7_COLOR_BAR, enable);
}

static int set_whitebal(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_DSP, CTRL1, CTRL1_AWB, enable);
}

static int set_gain_ctrl(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_SENSOR, COM8, COM8_AGC_EN, enable);
}

static int set_exposure_ctrl(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_SENSOR, COM8, COM8_AEC_EN, enable);
}

static int set_hmirror(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_SENSOR, REG04, REG04_HFLIP_IMG, enable);
}

static int set_vflip(sensor_t *sensor, int enable)
{
    return write_reg_bits(sensor, BANK_SEL_SENSOR, REG04, REG04_VFLIP_IMG, enable);
}

static int set_framerate(sensor_t *sensor, framerate_t framerate)
{
    return 0;
}

int ov2640_init(sensor_t *sensor)
{
    /* set function pointers */
    sensor->reset = reset;
    sensor->set_pixformat = set_pixformat;
    sensor->set_framesize = set_framesize;
    sensor->set_framerate = set_framerate;
    sensor->set_contrast  = set_contrast;
    sensor->set_brightness= set_brightness;
    sensor->set_saturation= set_saturation;
    sensor->set_gainceiling = set_gainceiling;
    sensor->set_quality = set_quality;
    sensor->set_colorbar = set_colorbar;
    sensor->set_gain_ctrl = set_gain_ctrl;
    sensor->set_exposure_ctrl = set_exposure_ctrl;
    sensor->set_whitebal = set_whitebal;
    sensor->set_hmirror = set_hmirror;
    sensor->set_vflip = set_vflip;

    // Set sensor flags
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_VSYNC, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_HSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_PIXCK, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_FSYNC, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_JPEGE, 0);

    return 0;
}
