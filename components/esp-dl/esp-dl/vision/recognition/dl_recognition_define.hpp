#pragma once
#include <cstdint>

namespace dl {
namespace recognition {
typedef enum { DB_FATFS_FLASH, DB_FATFS_SDCARD, DB_SPIFFS, DB_MAX = DB_SPIFFS } db_type_t;
typedef struct {
    uint16_t num_feats_total;
    uint16_t num_feats_valid;
    uint16_t feat_len;
} database_meta;

typedef struct {
    uint16_t id;
    float *feat;
} database_feat;

typedef struct {
    uint16_t id;
    float similarity;
} result_t;

} // namespace recognition
} // namespace dl
