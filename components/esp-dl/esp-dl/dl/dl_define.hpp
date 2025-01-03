#pragma once

#include "dl_define_private.hpp"
#include "sdkconfig.h"
#include <climits>
#include <string>

#define DL_LOG_LATENCY_UNIT 0  /*<! - 1: cycle */
                               /*<! - 0: us */
#define DL_LOG_NN_LATENCY 0    /*<! - 1: print the latency of each parts of nn */
                               /*<! - 0: mute */
#define DL_LOG_LAYER_LATENCY 0 /*<! - 1: print the latency of each parts of layer */
                               /*<! - 0: mute */
#define DL_LOG_CACHE_COUNT 0   /*<! - 1: print the cache hit/miss count only for esp32p4 */
                               /*<! - 0: mute */

#if CONFIG_SPIRAM_SUPPORT || CONFIG_ESP32_SPIRAM_SUPPORT || CONFIG_ESP32S2_SPIRAM_SUPPORT || \
    CONFIG_ESP32S3_SPIRAM_SUPPORT || CONFIG_SPIRAM
#define DL_SPIRAM_SUPPORT 1
#else
#define DL_SPIRAM_SUPPORT 0
#endif

#if CONFIG_IDF_TARGET_ESP32
#define CONFIG_DEFAULT_ASSIGN_CORE \
    {                              \
    } // TODO: 多核 task 完成时，改成默认 0,1
#elif CONFIG_IDF_TARGET_ESP32S2
#define CONFIG_DEFAULT_ASSIGN_CORE \
    {                              \
    }
#elif CONFIG_IDF_TARGET_ESP32S3
#define CONFIG_DEFAULT_ASSIGN_CORE \
    {                              \
    } // TODO: 多核 task 完成时，改成默认 0,1
#elif CONFIG_IDF_TARGET_ESP32C3
#define CONFIG_DEFAULT_ASSIGN_CORE \
    {                              \
    }
#else
#define CONFIG_DEFAULT_ASSIGN_CORE \
    {                              \
    }
#endif

#ifndef DL_EQUAL
#define DL_EQUAL(x, y) (((x) == (y)) ? (1) : (0))
#endif

#ifndef DL_MAX
#define DL_MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif

#ifndef DL_MIN
#define DL_MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef DL_CLIP
#define DL_CLIP(x, low, high) ((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x))
#endif

#ifndef DL_ABS
#define DL_ABS(x) ((x) < 0 ? (-(x)) : (x))
#endif

#ifndef DL_RIGHT_SHIFT
#define DL_RIGHT_SHIFT(x, shift) (((shift) > 0) ? ((x) >> (shift)) : ((x) << -(shift)))
#endif

#ifndef DL_LEFT_SHIFT
#define DL_LEFT_SHIFT(x, shift) (((shift) > 0) ? ((x) << (shift)) : ((x) >> -(shift)))
#endif

#ifndef DL_SCALE
#define DL_SCALE(exponent) (((exponent) > 0) ? (1 << (exponent)) : ((float)1.0 / (1 << -(exponent))))
#endif

#ifndef DL_RESCALE
#define DL_RESCALE(exponent) (((exponent) > 0) ? ((float)1.0 / (1 << (exponent))) : (1 << -(exponent)))
#endif

#define DL_QUANT8_MAX 127
#define DL_QUANT8_MIN -128
#define DL_QUANT16_MAX 32767
#define DL_QUANT16_MIN -32768
#define DL_QUANT32_MAX 2147483647
#define DL_QUANT32_MIN -2147483648

#define QIQO 0
#define QIFO 1

namespace dl {
typedef enum {
    QUANT_TYPE_NONE,       /*Unknown quantization type*/
    QUANT_TYPE_FLOAT32,    /*<! Float>*/
    QUANT_TYPE_SYMM_8BIT,  /*<! symmetry 8bit quantization (per tensor) >*/
    QUANT_TYPE_SYMM_16BIT, /*<! symmetry 16bit quantization (per tensor) >*/
    QUANT_TYPE_SYMM_32BIT, /*<! symmetry 32bit quantization (per tensor) >*/
} quant_type_t;

typedef enum {
    Linear,    /*<! Linear >*/
    ReLU,      /*<! ReLU >*/
    LeakyReLU, /*<! LeakyReLU >*/
    PReLU,     /*<! PReLU >*/
    // TODO: ReLU6
} activation_type_t;

typedef enum {
    PADDING_NOT_SET,
    PADDING_VALID,      /*<! no padding >*/
    PADDING_SAME_BEGIN, /*<! SAME in MXNET style >*/
    PADDING_SAME_END,   /*<! SAME in TensorFlow style >*/
} padding_type_t;

typedef enum {
    PADDING_EMPTY,
    PADDING_CONSTANT,
    PADDING_EDGE,
    PADDING_REFLECT,
    PADDING_WRAP,
} padding_mode_t;

typedef enum { RESIZE_NEAREST, RESIZE_LINEAR, RESIZE_CUBIC } resize_mode_t;

/**
 * @brief The mode of esp-dl runtime, single-core or multi-core
 */
typedef enum {
    RUNTIME_MODE_AUTO = 0,        // Automatically select single-core or multi-core runtime
    RUNTIME_MODE_SINGLE_CORE = 1, // Always select single-core runtime
    RUNTIME_MODE_MULTI_CORE = 2,  // Always select multi-core runtime(dual core for ESP32-S3 and ESP32-P4)
} runtime_mode_t;
} // namespace dl
