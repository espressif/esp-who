#include "fbs_loader.hpp"
#include "mbedtls/aes.h"

static const char *TAG = "FbsLoader";

namespace fbs {

/**
 * @brief This function is used to decrypt the AES 128-bit CTR mode encrypted data.
 * AES (Advanced Encryption Standard) is a widely-used symmetric encryption algorithm that provides strong security for
 * data protection CTR mode converts the block cipher into a stream cipher, allowing it to encrypt data of any length
 * without the need for padding
 *
 * @param ciphertext   Input Fbs data encrypted by AES 128-bit CTR mode
 * @param plaintext    Decrypted data
 * @param size         Size of input data
 * @param key          128-bit AES key
 */
void fbs_aes_crypt_ctr(const uint8_t *ciphertext, uint8_t *plaintext, size_t size, const uint8_t *key)
{
    mbedtls_aes_context aes_ctx;
    size_t offset = 0;
    uint8_t nonce[16] = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
    uint8_t stream_block[16];
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128); // 128-bit key
    mbedtls_aes_crypt_ctr(&aes_ctx, size, &offset, nonce, stream_block, ciphertext, plaintext);
    mbedtls_aes_free(&aes_ctx);
}

/**
    FBS_FILE_FORMAT_EDL1:
    {
        char[4]: "EDL1",
        uint32:  the mode of entru
        uint32:  the length of data
        uint8[]:  the data
    }

    FBS_FILE_FORMAT_PDL1:
    {
        "PDL1": char[4]
        model_num: uint32
        model1_data_offset: uint32
        model1_name_offset: uint32
        model1_name_length: uint32
        model2_data_offset: uint32
        model2_name_offset: uint32
        model2_name_length: uint32
        ...
        model1_name,
        model2_name,
        ...
        model1_data(format:FBS_FILE_FORMAT_EDL1),
        model2_data(format:FBS_FILE_FORMAT_EDL1),
        ...
    }

    FBS_FILE_FORMAT_EDL2:
    {
        char[4]: "EDL2",
        uint32:  the mode of entru
        uint32:  the length of data
        uint32:  zero padding
        uint8[]:  the data
        zero padding
    }

    FBS_FILE_FORMAT_PDL2:
    {
        "PDL2": char[4]
        model_num: uint32
        model1_data_offset: uint32
        model1_name_offset: uint32
        model1_name_length: uint32
        model2_data_offset: uint32
        model2_name_offset: uint32
        model2_name_length: uint32
        ...
        model1_name,
        model2_name,
        ...
        zero padding
        model1_data(format:FBS_FILE_FORMAT_EDL2),
        model2_data(format:FBS_FILE_FORMAT_EDL2),
        ...
    }
*/
typedef enum {
    FBS_FILE_FORMAT_UNK = 0,
    FBS_FILE_FORMAT_EDL1 = 1,
    FBS_FILE_FORMAT_PDL1 = 2,
    FBS_FILE_FORMAT_EDL2 = 3,
    FBS_FILE_FORMAT_PDL2 = 4
} fbs_file_format_t;

fbs_file_format_t get_model_format(const char *fbs_buf, model_location_type_t model_location)
{
    char str[5];
    if (model_location != MODEL_LOCATION_IN_SDCARD) {
        memcpy(str, fbs_buf, 4);
        str[4] = '\0';
    } else {
        FILE *f = fopen(fbs_buf, "rb");
        if (!f) {
            ESP_LOGE(TAG, "Failed to open %s.", fbs_buf);
            return FBS_FILE_FORMAT_UNK;
        }
        fread(str, 4, 1, f);
        str[4] = '\0';
        fclose(f);
    }

    if (strcmp(str, "EDL1") == 0) {
        return FBS_FILE_FORMAT_EDL1;
    } else if (strcmp(str, "PDL1") == 0) {
        return FBS_FILE_FORMAT_PDL1;
    } else if (strcmp(str, "EDL2") == 0) {
        return FBS_FILE_FORMAT_EDL2;
    } else if (strcmp(str, "PDL2") == 0) {
        return FBS_FILE_FORMAT_PDL2;
    } else {
        return FBS_FILE_FORMAT_UNK;
    }
}

esp_err_t get_model_offset_by_index(const char *fbs_buf,
                                    model_location_type_t model_location,
                                    uint32_t index,
                                    uint32_t &offset)
{
    if (model_location != MODEL_LOCATION_IN_SDCARD) {
        const uint32_t *header = (const uint32_t *)fbs_buf;
        uint32_t model_num = header[1];
        if (index >= model_num) {
            ESP_LOGE(TAG, "The model index is out of range.");
            return ESP_FAIL;
        }
        offset = header[2 + index * 3];
        return ESP_OK;
    } else {
        FILE *f = fopen(fbs_buf, "rb");
        if (!f) {
            ESP_LOGE(TAG, "Failed to open %s.", fbs_buf);
            return ESP_FAIL;
        }
        fseek(f, 4, SEEK_SET);
        uint32_t model_num;
        fread(&model_num, 4, 1, f);
        if (index >= model_num) {
            ESP_LOGE(TAG, "The model index is out of range.");
            fclose(f);
            return ESP_FAIL;
        }
        fseek(f, 4 * (2 + index * 3), SEEK_SET);
        fread(&offset, 4, 1, f);
        fclose(f);
        return ESP_OK;
    }
}

esp_err_t get_model_offset_by_name(const char *fbs_buf,
                                   model_location_type_t model_location,
                                   const char *name,
                                   uint32_t &offset)
{
    if (model_location != MODEL_LOCATION_IN_SDCARD) {
        const uint32_t *header = (const uint32_t *)fbs_buf;
        uint32_t model_num = header[1];
        uint32_t name_offset, name_length;
        for (int i = 0; i < model_num; i++) {
            name_offset = header[2 + 3 * i + 1];
            name_length = header[2 + 3 * i + 2];
            std::string model_name(fbs_buf + name_offset, name_length);
            if (model_name == std::string(name)) {
                offset = header[2 + 3 * i];
                return ESP_OK;
            }
        }
        ESP_LOGE(TAG, "Model %s is not found.", name);
        return ESP_FAIL;
    } else {
        FILE *f = fopen(fbs_buf, "rb");
        if (!f) {
            ESP_LOGE(TAG, "Failed to open %s.", fbs_buf);
            return ESP_FAIL;
        }
        fseek(f, 4, SEEK_SET);
        uint32_t model_num;
        fread(&model_num, 4, 1, f);
        uint32_t name_offset, name_length;
        for (int i = 0; i < model_num; i++) {
            fseek(f, 4 * (2 + 3 * i + 1), SEEK_SET);
            fread(&name_offset, 4, 1, f);
            fread(&name_length, 4, 1, f);
            std::string model_name(name_length, '\0');
            fseek(f, name_offset, SEEK_SET);
            fread(model_name.data(), name_length, 1, f);
            if (model_name == std::string(name)) {
                fseek(f, 4 * (2 + 3 * i), SEEK_SET);
                fread(&offset, 4, 1, f);
                fclose(f);
                return ESP_OK;
            }
        }
        ESP_LOGE(TAG, "Model %s is not found.", name);
        fclose(f);
        return ESP_FAIL;
    }
}

FbsModel *create_fbs_model(const char *fbs_buf,
                           fbs_file_format_t format,
                           model_location_type_t model_location,
                           uint32_t offset,
                           const uint8_t *key,
                           bool param_copy)
{
    if (fbs_buf == nullptr) {
        ESP_LOGE(TAG, "Model's flatbuffers is empty.");
        return nullptr;
    }

    char *model_buf;
    uint32_t mode, size;
    if (model_location != MODEL_LOCATION_IN_SDCARD) {
        model_buf = const_cast<char *>(fbs_buf + offset);
        uint32_t *header = (uint32_t *)model_buf;
        mode = header[1]; // cryptographic mode, 0: without encryption, 1: aes encryption
        size = header[2];
        if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_PDL1) {
            model_buf += 12;
        } else {
            model_buf += 16;
        }
    } else {
        FILE *f = fopen(fbs_buf, "rb");
        if (!f) {
            ESP_LOGE(TAG, "Failed to open %s.", fbs_buf);
            return nullptr;
        }
        fseek(f, offset + 4, SEEK_SET);
        fread(&mode, 4, 1, f);
        fread(&size, 4, 1, f);
        model_buf = (char *)dl::tool::malloc_aligned(size, 1, 16, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        if (format == FBS_FILE_FORMAT_EDL2 || format == FBS_FILE_FORMAT_PDL2) {
            fseek(f, 4, SEEK_CUR);
        }
        fread(model_buf, size, 1, f);
    }

    if (mode != 0 && key == NULL) {
        ESP_LOGE(TAG, "This is a cryptographic model, please enter the secret key!");
        return nullptr;
    }

    if (mode == 0) { // without encryption
        bool auto_free, real_param_copy;
        if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_PDL1) {
            if (model_location == MODEL_LOCATION_IN_SDCARD) {
                auto_free = true;
                real_param_copy = true;
            } else {
                auto_free = false;
                real_param_copy = true;
            }
        } else {
            if (model_location == MODEL_LOCATION_IN_SDCARD) {
                auto_free = true;
                real_param_copy = false;
            } else {
                auto_free = false;
                bool address_align = !(reinterpret_cast<uintptr_t>(model_buf) & 0xf);
                real_param_copy = param_copy;
                if (!param_copy && !address_align) {
                    ESP_LOGW(
                        TAG,
                        "Failed to set param_copy to false, The address of fbs model is not aligned with 16 bytes.");
                    real_param_copy = true;
                }
#if CONFIG_SPIRAM_RODATA
                if ((model_location == MODEL_LOCATION_IN_FLASH_RODATA) && param_copy && address_align) {
                    real_param_copy = false;
                }
#endif
            }
        }
        return new FbsModel(model_buf, auto_free, real_param_copy);
    } else if (mode == 1) { // 128-bit AES encryption
        uint8_t *m_data = (uint8_t *)dl::tool::malloc_aligned(size, 1, 16, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        fbs_aes_crypt_ctr((const uint8_t *)model_buf, m_data, size, key);
        if (model_location == MODEL_LOCATION_IN_SDCARD) {
            heap_caps_free(model_buf);
        }
        if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_PDL1) {
            return new FbsModel(m_data, true, true);
        } else {
            return new FbsModel(m_data, true, false);
        }
    }

    return nullptr;
}

FbsLoader::FbsLoader(const char *name, model_location_type_t location) :
    m_mmap_handle(nullptr), m_location(location), m_fbs_buf(nullptr)
{
    if (name == nullptr) {
        return;
    }

    if (m_location == MODEL_LOCATION_IN_FLASH_RODATA || m_location == MODEL_LOCATION_IN_SDCARD) {
        m_fbs_buf = (const void *)name;
    } else if (m_location == MODEL_LOCATION_IN_FLASH_PARTITION) {
        const esp_partition_t *partition =
            esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, name);
        if (partition) {
            int free_pages = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
            uint32_t storage_size = free_pages * 64 * 1024; // Byte
            ESP_LOGI(TAG, "The storage free size is %ld KB", storage_size / 1024);
            ESP_LOGI(TAG, "The partition size is %ld KB", partition->size / 1024);
            if (storage_size < partition->size) {
                ESP_LOGE(TAG,
                         "The storage free size of this board is less than %s partition required size",
                         partition->label);
            }
            this->m_mmap_handle = (esp_partition_mmap_handle_t *)malloc(sizeof(esp_partition_mmap_handle_t));
            ESP_ERROR_CHECK(esp_partition_mmap(partition,
                                               0,
                                               partition->size,
                                               ESP_PARTITION_MMAP_DATA,
                                               &this->m_fbs_buf,
                                               static_cast<esp_partition_mmap_handle_t *>(this->m_mmap_handle)));
        } else {
            ESP_LOGE(TAG, "Can not find %s in partition table", name);
        }
    }
}

FbsLoader::~FbsLoader()
{
    if (m_location == MODEL_LOCATION_IN_FLASH_PARTITION) {
        esp_partition_munmap(*static_cast<esp_partition_mmap_handle_t *>(this->m_mmap_handle)); // support esp-idf v5
        if (this->m_mmap_handle) {
            free(this->m_mmap_handle);
            this->m_mmap_handle = nullptr;
        }
    }
}

FbsModel *FbsLoader::load(const int model_index, const uint8_t *key, bool param_copy)
{
    if (this->m_fbs_buf == nullptr) {
        ESP_LOGE(TAG, "Model's flatbuffers is empty.");
        return nullptr;
    }

    uint32_t offset = 0;
    fbs_file_format_t format = get_model_format((const char *)m_fbs_buf, m_location);
    if (format == FBS_FILE_FORMAT_PDL1 || format == FBS_FILE_FORMAT_PDL2) {
        // packed multiple espdl models
        if (get_model_offset_by_index((const char *)m_fbs_buf, m_location, model_index, offset) != ESP_OK) {
            return nullptr;
        }
    } else if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_EDL2) {
        // single espdl model
        if (model_index > 0) {
            ESP_LOGW(TAG, "There is only one model in the flatbuffers, ignore the input model index!");
        }
        offset = 0;
    } else {
        ESP_LOGE(TAG, "Unsupported format, or the model file is corrupted!");
        return nullptr;
    }
    return create_fbs_model((const char *)m_fbs_buf, format, m_location, offset, key, param_copy);
}

FbsModel *FbsLoader::load(const uint8_t *key, bool param_copy)
{
    return this->load(0, key, param_copy);
}

FbsModel *FbsLoader::load(const char *model_name, const uint8_t *key, bool param_copy)
{
    if (this->m_fbs_buf == nullptr) {
        ESP_LOGE(TAG, "Model's flatbuffers is empty.");
        return nullptr;
    }

    uint32_t offset = 0;
    fbs_file_format_t format = get_model_format((const char *)m_fbs_buf, m_location);
    if (format == FBS_FILE_FORMAT_PDL1 || format == FBS_FILE_FORMAT_PDL2) {
        // packed multiple espdl models
        if (get_model_offset_by_name((const char *)m_fbs_buf, m_location, model_name, offset) != ESP_OK) {
            return nullptr;
        }
    } else if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_EDL2) {
        // single espdl model
        if (model_name) {
            ESP_LOGW(TAG, "There is only one model in the flatbuffers, ignore the input model name!");
        }
        offset = 0;
    } else {
        ESP_LOGE(TAG, "Unsupported format, or the model file is corrupted!");
        return nullptr;
    }
    return create_fbs_model((const char *)m_fbs_buf, format, m_location, offset, key, param_copy);
}

int FbsLoader::get_model_num()
{
    if (this->m_fbs_buf == nullptr) {
        return 0;
    }

    fbs_file_format_t format = get_model_format((const char *)m_fbs_buf, m_location);
    if (format == FBS_FILE_FORMAT_PDL1 || format == FBS_FILE_FORMAT_PDL2) {
        // packed multiple espdl models
        uint32_t model_num;
        if (m_location != MODEL_LOCATION_IN_SDCARD) {
            uint32_t *header = (uint32_t *)m_fbs_buf;
            model_num = header[1];
        } else {
            FILE *f = fopen((const char *)m_fbs_buf, "rb");
            if (!f) {
                ESP_LOGE(TAG, "Failed to open %s.", (const char *)m_fbs_buf);
                return 0;
            }
            fseek(f, 4, SEEK_SET);
            fread(&model_num, 4, 1, f);
            fclose(f);
        }
        return model_num;
    } else if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_EDL2) {
        // single espdl model
        return 1;
    } else {
        ESP_LOGE(TAG, "Unsupported format, or the model file is corrupted!");
        return 0;
    }

    return 0;
}

void FbsLoader::list_models()
{
    if (this->m_fbs_buf == nullptr) {
        ESP_LOGE(TAG, "Model's flatbuffers is empty.");
        return;
    }

    fbs_file_format_t format = get_model_format((const char *)m_fbs_buf, m_location);
    if (format == FBS_FILE_FORMAT_PDL1 || format == FBS_FILE_FORMAT_PDL2) {
        // packed multiple espdl models
        if (m_location != MODEL_LOCATION_IN_SDCARD) {
            uint32_t *header = (uint32_t *)m_fbs_buf;
            uint32_t model_num = header[1];
            for (int i = 0; i < model_num; i++) {
                uint32_t name_offset = header[2 + 3 * i + 1];
                uint32_t name_length = header[2 + 3 * i + 2];
                std::string name((const char *)m_fbs_buf + name_offset, name_length);
                ESP_LOGI(TAG, "model name: %s, index:%d", name.c_str(), i);
            }
        } else {
            FILE *f = fopen((const char *)m_fbs_buf, "rb");
            if (!f) {
                ESP_LOGE(TAG, "Failed to open %s.", (const char *)m_fbs_buf);
                return;
            }
            fseek(f, 4, SEEK_SET);
            uint32_t model_num;
            fread(&model_num, 4, 1, f);
            uint32_t name_offset, name_length;
            for (int i = 0; i < model_num; i++) {
                fseek(f, 4 * (2 + 3 * i + 1), SEEK_SET);
                fread(&name_offset, 4, 1, f);
                fread(&name_length, 4, 1, f);
                std::string name(name_length, '\0');
                fseek(f, name_offset, SEEK_SET);
                fread(name.data(), name_length, 1, f);
                ESP_LOGI(TAG, "model name: %s, index:%d", name.c_str(), i);
            }
            fclose(f);
        }
    } else if (format == FBS_FILE_FORMAT_EDL1 || format == FBS_FILE_FORMAT_EDL2) {
        ESP_LOGI(TAG, "There is only one model in the flatbuffers without model name.");
    }
}

} // namespace fbs
