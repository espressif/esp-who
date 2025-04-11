#pragma once
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "who_s3_cam.hpp"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "who_p4_cam.hpp"
#include "who_p4_ppa_cam.hpp"
#endif
