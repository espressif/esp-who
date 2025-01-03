#pragma once

#include "cmath"
#include "dl_image.hpp"
#include "dl_model_base.hpp"
#include "dl_tensor_base.hpp"
#include "esp_cache.h"
#include "stdint.h"
#include "driver/ppa.h"
#include "esp_private/esp_cache_private.h"

namespace dl {
namespace image {
/**
 * @brief rgb565->rgb888, crop, resize, normalize, quantize
 */
class ImagePreprocessor {
public:
    TensorBase *m_model_input;

private:
    const std::vector<float> m_mean;
    const std::vector<float> m_std;
    uint32_t m_caps;
    void *m_norm_lut;
    std::vector<int> m_crop_area;
    float m_resize_scale_x;
    float m_resize_scale_y;
    img_t m_output;
#if CONFIG_IDF_TARGET_ESP32P4
    ppa_client_handle_t m_ppa_srm_handle;
    size_t m_ppa_buffer_size;
    void *m_ppa_buffer;
#endif
    template <typename T>
    void create_norm_lut();

public:
    ImagePreprocessor(Model *model,
                      const std::vector<float> &mean,
                      const std::vector<float> &std,
                      uint32_t caps = 0,
                      const std::string &input_name = "");

    ~ImagePreprocessor();

    float get_resize_scale_x() { return m_resize_scale_x; };
    float get_resize_scale_y() { return m_resize_scale_y; };
    float get_top_left_x() { return m_crop_area[0]; };
    float get_top_left_y() { return m_crop_area[1]; };

    void preprocess(const img_t &img, const std::vector<int> &crop_area = {});
    void preprocess(const img_t &img, uint16_t rescaled_w, uint16_t rescaled_h, const std::vector<int> &crop_area = {});
    void preprocess(const img_t &img, dl::math::Matrix<float> *M_inv);
};

} // namespace image
} // namespace dl
