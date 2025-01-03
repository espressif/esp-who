#pragma once
#include "dl_image_preprocessor.hpp"
#include "dl_math_matrix.hpp"

namespace dl {
namespace image {
class FeatImagePreprocessor {
public:
    FeatImagePreprocessor(Model *model,
                          const std::vector<float> &mean,
                          const std::vector<float> &std,
                          uint32_t caps = 0,
                          const std::string &input_name = "") :
        m_image_preprocessor(new dl::image::ImagePreprocessor(model, mean, std, caps, input_name)) {};

    ~FeatImagePreprocessor();

    void preprocess(const dl::image::img_t &img, const std::vector<int> &landmarks);

private:
    static std::vector<float> s_std_ldks_112;
    dl::image::ImagePreprocessor *m_image_preprocessor;
};
} // namespace image
} // namespace dl
