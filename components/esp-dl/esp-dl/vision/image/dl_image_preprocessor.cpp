#include "dl_image_preprocessor.hpp"

namespace dl {
namespace image {

ImagePreprocessor::ImagePreprocessor(Model *model,
                                     const std::vector<float> &mean,
                                     const std::vector<float> &std,
                                     uint32_t caps,
                                     const std::string &input_name) :
    m_mean(mean), m_std(std), m_caps(caps)
{
    if (input_name.empty()) {
        std::map<std::string, dl::TensorBase *> model_inputs_map = model->get_inputs();
        assert(model_inputs_map.size() == 1);
        m_model_input = model_inputs_map.begin()->second;
    } else {
        m_model_input = model->get_intermediate(input_name);
    }
    assert(m_model_input->dtype == DATA_TYPE_INT8 || m_model_input->dtype == DATA_TYPE_INT16);
    assert(m_model_input->shape[3] == m_mean.size() && m_mean.size() == m_std.size());
    m_output = {.data = m_model_input->data,
                .width = m_model_input->shape[2],
                .height = m_model_input->shape[1],
                .pix_type = (m_model_input->dtype == DATA_TYPE_INT8) ? DL_IMAGE_PIX_TYPE_RGB888_QINT8
                                                                     : DL_IMAGE_PIX_TYPE_RGB888_QINT16};
    if (m_model_input->dtype == DATA_TYPE_INT8) {
        create_norm_lut<int8_t>();
    } else {
        create_norm_lut<int16_t>();
    }
#if CONFIG_IDF_TARGET_ESP32P4
    if (m_caps & DL_IMAGE_CAP_PPA) {
        ppa_client_config_t ppa_client_config;
        memset(&ppa_client_config, 0, sizeof(ppa_client_config_t));
        ppa_client_config.oper_type = PPA_OPERATION_SRM;
        ESP_ERROR_CHECK(ppa_register_client(&ppa_client_config, &m_ppa_srm_handle));
        size_t cache_line_size;
        ESP_ERROR_CHECK(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &cache_line_size));
        m_ppa_buffer_size = DL_IMAGE_ALIGN_UP(
            m_model_input->shape[1] * m_model_input->shape[2] * m_model_input->shape[3], cache_line_size);
        m_ppa_buffer = tool::calloc_aligned(
            m_ppa_buffer_size, sizeof(uint8_t), cache_line_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    }
#endif
}

ImagePreprocessor::~ImagePreprocessor()
{
    if (m_norm_lut) {
        heap_caps_free(m_norm_lut);
        m_norm_lut = nullptr;
    }
#if CONFIG_IDF_TARGET_ESP32P4
    if (m_caps & DL_IMAGE_CAP_PPA) {
        if (m_ppa_buffer) {
            heap_caps_free(m_ppa_buffer);
            m_ppa_buffer = nullptr;
        }
        if (m_ppa_srm_handle) {
            ESP_ERROR_CHECK(ppa_unregister_client(m_ppa_srm_handle));
            m_ppa_srm_handle = nullptr;
        }
    }
#endif
}

template <typename T>
void ImagePreprocessor::create_norm_lut()
{
    m_norm_lut = tool::malloc_aligned(m_mean.size() * 256, sizeof(T), 16, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    float inv_scale = 1.f / DL_SCALE(m_model_input->exponent);
    std::vector<float> inv_std(m_std.size());
    T *norm_lut_ptr = (T *)m_norm_lut;
    for (int i = 0; i < m_mean.size(); i++) {
        inv_std[i] = 1.f / m_std[i];
        for (int j = 0; j < 256; j++) {
            norm_lut_ptr[i * 256 + j] = quantize<T>(((float)j - m_mean[i]) * inv_std[i], inv_scale);
        }
    }
}

template void ImagePreprocessor::create_norm_lut<int8_t>();
template void ImagePreprocessor::create_norm_lut<int16_t>();

void ImagePreprocessor::preprocess(const img_t &img, const std::vector<int> &crop_area)
{
    assert(get_img_channel(img) == m_mean.size());
    m_crop_area = crop_area;
#if CONFIG_IDF_TARGET_ESP32P4
    if (resize_ppa(img,
                   m_output,
                   m_ppa_srm_handle,
                   m_ppa_buffer,
                   m_ppa_buffer_size,
                   PPA_TRANS_MODE_BLOCKING,
                   nullptr,
                   m_caps,
                   m_norm_lut,
                   crop_area,
                   &m_resize_scale_x,
                   &m_resize_scale_y) == ESP_FAIL) {
        resize(img,
               m_output,
               DL_IMAGE_INTERPOLATE_NEAREST,
               m_caps,
               m_norm_lut,
               crop_area,
               &m_resize_scale_x,
               &m_resize_scale_y);
    }
#else
    resize(img,
           m_output,
           DL_IMAGE_INTERPOLATE_NEAREST,
           m_caps,
           m_norm_lut,
           crop_area,
           &m_resize_scale_x,
           &m_resize_scale_y);
#endif
}

void ImagePreprocessor::preprocess(const img_t &img, dl::math::Matrix<float> *M_inv)
{
    assert(get_img_channel(img) == m_mean.size());
    warp_affine(img, m_output, DL_IMAGE_INTERPOLATE_NEAREST, M_inv, m_caps, m_norm_lut);
}
} // namespace image
} // namespace dl
