/**
 * @file dl_define_private.hpp
 * @brief All macro here is for internal only. Once the project is compiled to static library, these macro is not
 * effective.
 * @version 0.1
 * @date 2021-07-02
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#include "sdkconfig.h"

#define DL_S16_BUFFER_TYPE                                                                                        \
    int64_t /*<! int32_t or int64_t. int32_t is twice as fast as int64_t in C/C++ implement. But int32_t may have \
               value overflow. >*/
#define DL_LOG_DETECT_LATENCY 0 /*<! - 1: print the latency of each parts of detect */
                                /*<! - 0: mute */

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define CONFIG_XTENSA_BOOST 1
#else
#define CONFIG_XTENSA_BOOST 0
#endif

#if CONFIG_IDF_TARGET_ESP32S3
#define CONFIG_TIE728_BOOST 1
#else
#define CONFIG_TIE728_BOOST 0
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#define CONFIG_ESP32P4_BOOST 1
#else
#define CONFIG_ESP32P4_BOOST 0
#endif

#define CONFIG_ACCURATE_INFER 1

#define CONFIG_DL_DEBUG 0
