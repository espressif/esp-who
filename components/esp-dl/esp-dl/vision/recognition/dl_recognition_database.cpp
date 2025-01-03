#include "dl_recognition_database.hpp"
#include <unistd.h>

static const char *TAG = "dl::recognition::DataBase";

namespace dl {
namespace recognition {
DataBase::DataBase(const char *db_path, db_type_t db_type, int feat_len) : m_db_type(db_type)
{
    assert(db_path);
    int length = strlen(db_path) + 1;
    m_db_path = (char *)malloc(sizeof(char) * length);
    memcpy(m_db_path, db_path, length);
    if (access(db_path, F_OK) == 0) {
        load_database_from_storage(feat_len);
    } else {
        create_empty_database_in_storage(feat_len);
    }
}

DataBase::~DataBase()
{
    clear_all_feats_in_memory();
    if (m_db_path) {
        free(m_db_path);
        m_db_path = nullptr;
    }
}

esp_err_t DataBase::create_empty_database_in_storage(int feat_len)
{
    FILE *f = fopen(m_db_path, "wb");
    size_t size = 0;
    if (!f) {
        ESP_LOGE(TAG, "Failed to open db.");
        return ESP_FAIL;
    }
    m_meta.num_feats_total = 0;
    m_meta.num_feats_valid = 0;
    m_meta.feat_len = feat_len;
    size = fwrite(&m_meta, sizeof(database_meta), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write db meta data.");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t DataBase::clear_all_feats()
{
    if (remove(m_db_path) == -1) {
        ESP_LOGE(TAG, "Failed to remove db.");
        return ESP_FAIL;
    }
    ESP_RETURN_ON_ERROR(
        create_empty_database_in_storage(m_meta.feat_len), TAG, "Failed to create empty db in storage.");
    clear_all_feats_in_memory();
    return ESP_OK;
}

void DataBase::clear_all_feats_in_memory()
{
    for (auto it = m_feats.begin(); it != m_feats.end(); it++) {
        heap_caps_free(it->feat);
    }
    m_feats.clear();
    m_meta.num_feats_total = 0;
    m_meta.num_feats_valid = 0;
}

esp_err_t DataBase::load_database_from_storage(int feat_len)
{
    clear_all_feats_in_memory();
    FILE *f = fopen(m_db_path, "rb");
    size_t size = 0;
    if (!f) {
        ESP_LOGE(TAG, "Failed to open db.");
        return ESP_FAIL;
    }
    size = fread(&m_meta, sizeof(database_meta), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to read database meta.");
        fclose(f);
        return ESP_FAIL;
    }
    if (feat_len != m_meta.feat_len) {
        ESP_LOGE(TAG, "Feature len in storage does not match feature len in db.");
        fclose(f);
        return ESP_FAIL;
    }
    uint16_t id;
    for (int i = 0; i < m_meta.num_feats_total; i++) {
        size = fread(&id, sizeof(uint16_t), 1, f);
        if (size != 1) {
            ESP_LOGE(TAG, "Failed to read feature id.");
            fclose(f);
            return ESP_FAIL;
        }
        if (id == 0) {
            if (fseek(f, sizeof(float) * m_meta.feat_len, SEEK_CUR) != 0) {
                ESP_LOGE(TAG, "Failed to seek db file.");
                fclose(f);
                return ESP_FAIL;
            }
            continue;
        }
        float *feat =
            (float *)tool::malloc_aligned(m_meta.feat_len, sizeof(float), 16, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        size = fread(feat, sizeof(float), m_meta.feat_len, f);
        if (size != m_meta.feat_len) {
            ESP_LOGE(TAG, "Failed to read feature data.");
            fclose(f);
            return ESP_FAIL;
        }
        m_feats.emplace_back(id, feat);
    }
    if (m_feats.size() != m_meta.num_feats_valid) {
        ESP_LOGE(TAG, "Incorrect valid feature num.");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t DataBase::enroll_feat(TensorBase *feat)
{
    if (feat->dtype != DATA_TYPE_FLOAT) {
        ESP_LOGE(TAG, "Only support float feature.");
        return ESP_FAIL;
    }
    if (feat->size != m_meta.feat_len) {
        ESP_LOGE(TAG, "Feature len to enroll does not match feature len in db.");
        return ESP_FAIL;
    }
    float *feat_copy =
        (float *)tool::malloc_aligned(m_meta.feat_len, sizeof(float), 16, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    memcpy(feat_copy, feat->data, feat->get_bytes());

    m_feats.emplace_back(m_meta.num_feats_total + 1, feat_copy);
    m_meta.num_feats_total++;
    m_meta.num_feats_valid++;

    size_t size = 0;
    FILE *f = fopen(m_db_path, "rb+");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open db.");
        return ESP_FAIL;
    }
    size = fwrite(&m_meta, sizeof(database_meta), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write database meta.");
        fclose(f);
        return ESP_FAIL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        ESP_LOGE(TAG, "Failed to seek db file.");
        fclose(f);
        return ESP_FAIL;
    }
    size = fwrite(&m_feats.back().id, sizeof(uint16_t), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write feature id.");
        fclose(f);
        return ESP_FAIL;
    }
    size = fwrite(m_feats.back().feat, sizeof(float), m_meta.feat_len, f);
    if (size != m_meta.feat_len) {
        ESP_LOGE(TAG, "Failed to write feature.");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t DataBase::delete_feat(uint16_t id)
{
    bool invalid_id = true;
    for (auto it = m_feats.begin(); it != m_feats.end();) {
        if (it->id != id) {
            it++;
        } else {
            heap_caps_free(it->feat);
            it = m_feats.erase(it);
            m_meta.num_feats_valid--;
            invalid_id = false;
            break;
        }
    }
    if (invalid_id) {
        ESP_LOGW(TAG, "Invalid id to delete.");
        return ESP_FAIL;
    }
    size_t size = 0;
    FILE *f = fopen(m_db_path, "rb+");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open db.");
        return ESP_FAIL;
    }
    long int offset = sizeof(database_meta) + (sizeof(uint16_t) + sizeof(float) * m_meta.feat_len) * (id - 1);
    uint16_t id_invalid = 0;
    if (fseek(f, offset, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "Failed to seek db file.");
        fclose(f);
        return ESP_FAIL;
    }
    size = fwrite(&id_invalid, sizeof(uint16_t), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write feature id.");
        fclose(f);
        return ESP_FAIL;
    }

    offset = sizeof(uint16_t);
    if (fseek(f, offset, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "Failed to seek db file.");
        fclose(f);
        return ESP_FAIL;
    }
    size = fwrite(&m_meta.num_feats_valid, sizeof(uint16_t), 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write valid feature num.");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}

esp_err_t DataBase::delete_last_feat()
{
    if (m_feats.empty()) {
        ESP_LOGW(TAG, "Empty db, nothing to delete");
        return ESP_FAIL;
    }
    uint16_t id = m_feats.back().id;
    return delete_feat(id);
}

float DataBase::cal_similarity(float *feat1, float *feat2)
{
    float sum = 0;
    for (int i = 0; i < m_meta.feat_len; i++) {
        sum += feat1[i] * feat2[i];
    }
    return sum;
}

std::vector<result_t> DataBase::query_feat(TensorBase *feat, float thr, int top_k)
{
    if (top_k < 1) {
        ESP_LOGW(TAG, "Top_k should be greater than 0.");
        return {};
    }
    std::vector<result_t> results;
    float sim;
    int i = 1;
    for (auto it = m_feats.begin(); it != m_feats.end(); it++, i++) {
        sim = cal_similarity(it->feat, (float *)feat->data);
        if (sim <= thr) {
            continue;
        }
        // results.emplace_back(it->id, sim);
        results.emplace_back(i, sim);
    }
    std::sort(results.begin(), results.end(), [](const result_t &a, const result_t &b) -> bool {
        return a.similarity > b.similarity;
    });
    if (results.size() > top_k) {
        results.resize(top_k);
    }
    return results;
}

void DataBase::print()
{
    printf("\n");
    printf("[db meta]\nnum_feats_total: %d, num_feats_valid: %d, feat_len: %d\n",
           m_meta.num_feats_total,
           m_meta.num_feats_valid,
           m_meta.feat_len);
    printf("[feats]\n");
    for (auto it : m_feats) {
        printf("id: %d feat: ", it.id);
        for (int i = 0; i < m_meta.feat_len; i++) {
            printf("%f, ", it.feat[i]);
        }
        printf("\n");
    }
    printf("\n");
}

} // namespace recognition
} // namespace dl
