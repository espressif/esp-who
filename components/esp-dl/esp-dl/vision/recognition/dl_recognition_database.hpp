#pragma once
#include "dl_recognition_define.hpp"
#include "dl_tensor_base.hpp"
#include "esp_check.h"
#include "esp_system.h"
#include <algorithm>
#include <list>

namespace dl {
namespace recognition {
class DataBase {
public:
    DataBase(const char *db_path, db_type_t db_type, int feat_len);
    virtual ~DataBase();
    esp_err_t clear_all_feats();
    esp_err_t enroll_feat(TensorBase *feat);
    esp_err_t delete_feat(uint16_t id);
    esp_err_t delete_last_feat();
    std::vector<result_t> query_feat(TensorBase *feat, float thr, int top_k);
    void print();
    int get_num_feats() { return m_meta.num_feats_valid; }

private:
    char *m_db_path;
    db_type_t m_db_type;
    std::list<database_feat> m_feats;
    database_meta m_meta;

    esp_err_t create_empty_database_in_storage(int feat_len);
    esp_err_t load_database_from_storage(int feat_len);
    void clear_all_feats_in_memory();
    float cal_similarity(float *feat1, float *feat2);
};
} // namespace recognition
} // namespace dl
