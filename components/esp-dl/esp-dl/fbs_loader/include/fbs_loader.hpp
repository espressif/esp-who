#pragma once

#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_partition.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "spi_flash_mmap.h"
#endif
#include "fbs_model.hpp"

namespace fbs {

typedef enum {
    MODEL_LOCATION_IN_FLASH_RODATA = 0,    // The model in FLASH .rodata section
    MODEL_LOCATION_IN_FLASH_PARTITION = 1, // The model in SPIFFS
    MODEL_LOCATION_IN_SDCARD = 2,          // The model in SDCard
    MODEL_LOCATION_MAX = MODEL_LOCATION_IN_SDCARD,
} model_location_type_t;

/**
 * @brief Class for parser the flatbuffers.
 *
 */
class FbsLoader {
public:
    /**
     * @brief Construct a new FbsLoader object.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location  The model location.
     */
    FbsLoader(const char *rodata_address_or_partition_label_or_path = nullptr,
              model_location_type_t location = MODEL_LOCATION_IN_FLASH_RODATA);

    /**
     * @brief Destroy the FbsLoader object.
     */
    ~FbsLoader();

    /**
     * @brief Load the model. If there are multiple sub-models, the first sub-model will be loaded.
     *
     * @param key   NULL or a 128-bit AES key, like {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     * 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     *
     * @return  Return nullptr if loading fails. Otherwise return the pointer of FbsModel.
     */
    FbsModel *load(const uint8_t *key = nullptr, bool param_copy = true);

    /**
     * @brief Load the model by model index.
     *
     * @param model_index  The index of model.
     * @param key   NULL or a 128-bit AES key, like {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     * 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     *
     * @return  Return nullptr if loading fails. Otherwise return the pointer of FbsModel.
     */
    FbsModel *load(const int model_index, const uint8_t *key = nullptr, bool param_copy = true);

    /**
     * @brief Load the model by model name.
     *
     * @param model_name  The name of model.
     * @param key   NULL or a 128-bit AES key, like {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
     * 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     *
     * @return  Return nullptr if loading fails. Otherwise return the pointer of FbsModel.
     */
    FbsModel *load(const char *model_name, const uint8_t *key = nullptr, bool param_copy = true);

    /**
     * @brief Get the number of models.
     *
     * @return The number of models
     */
    int get_model_num();

    /**
     * @brief List all model's name
     */
    void list_models();

private:
    void *m_mmap_handle;
    model_location_type_t m_location;
    const void *m_fbs_buf;
};

} // namespace fbs
