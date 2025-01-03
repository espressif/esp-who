#include "dl_feat_image_preprocessor.hpp"

namespace dl {
namespace image {
std::vector<float> FeatImagePreprocessor::s_std_ldks_112 = {
    38.2946, 51.6963, 41.5493, 92.3655, 56.0252, 71.7366, 73.5318, 51.5014, 70.7299, 92.2041};

FeatImagePreprocessor::~FeatImagePreprocessor()
{
    if (m_image_preprocessor) {
        delete m_image_preprocessor;
        m_image_preprocessor = nullptr;
    }
}

void FeatImagePreprocessor::preprocess(const dl::image::img_t &img, const std::vector<int> &landmarks)
{
    assert(landmarks.size() == 10);
    // align face
    float h_scale = (float)m_image_preprocessor->m_model_input->shape[1] / 112.0;
    float w_scale = (float)m_image_preprocessor->m_model_input->shape[2] / 112.0;
    dl::math::Matrix<float> source_coord(5, 2);
    dl::math::Matrix<float> dest_coord(5, 2);
    dest_coord.set_value(landmarks);
    for (int i = 0; i < source_coord.h; i++) {
        source_coord.array[i][0] = w_scale * s_std_ldks_112[2 * i];
        source_coord.array[i][1] = h_scale * s_std_ldks_112[2 * i + 1];
    }
    dl::math::Matrix<float> M_inv = dl::math::get_similarity_transform(source_coord, dest_coord);
    m_image_preprocessor->preprocess(img, &M_inv);
}
} // namespace image
} // namespace dl
