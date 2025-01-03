#pragma once

#include "dl_detect_define.hpp"
#include "dl_feat_base.hpp"
#include "dl_recognition_database.hpp"
#include "dl_tensor_base.hpp"
namespace human_face_recognition {
class MFN : public dl::feat::FeatImpl {
public:
    MFN(const char *model_name);
};

using MBF = MFN;
} // namespace human_face_recognition

class HumanFaceFeat : public dl::feat::FeatWrapper {
public:
    typedef enum {
        MFN_S8_V1,
        MBF_S8_V1,
    } model_type_t;
    HumanFaceFeat(const char *sdcard_model_dir = nullptr,
                  model_type_t model_type = static_cast<model_type_t>(CONFIG_HUMAN_FACE_FEAT_MODEL_TYPE));
};

class HumanFaceRecognizer : public dl::recognition::DataBase {
private:
    HumanFaceFeat *m_feat_extract;
    float m_thr;
    int m_top_k;

public:
    HumanFaceRecognizer(HumanFaceFeat *feat_model,
                        char *db_path,
                        dl::recognition::db_type_t db_type = dl::recognition::DB_FATFS_FLASH,
                        float thr = 0.5,
                        int top_k = 1) :
        dl::recognition::DataBase(db_path, db_type, 512), m_feat_extract(feat_model), m_thr(thr), m_top_k(top_k)
    {
    }

    std::vector<dl::recognition::result_t> recognize(const dl::image::img_t &img,
                                                     std::list<dl::detect::result_t> &detect_res);
    esp_err_t enroll(const dl::image::img_t &img, std::list<dl::detect::result_t> &detect_res);
};
