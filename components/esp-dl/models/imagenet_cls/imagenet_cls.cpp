#include "imagenet_cls.hpp"

#if CONFIG_IMAGENET_CLS_MODEL_IN_FLASH_RODATA
extern const uint8_t imagenet_cls_espdl[] asm("_binary_imagenet_cls_espdl_start");
static const char *path = (const char *)imagenet_cls_espdl;
#elif CONFIG_IMAGENET_CLS_MODEL_IN_FLASH_PARTITION
static const char *path = "imagenet_cls";
#endif
namespace imagenet_cls {

MobileNetV2::MobileNetV2(const char *model_name, const int top_k)
{
    m_model =
        new dl::Model(path, model_name, static_cast<fbs::model_location_type_t>(CONFIG_IMAGENET_CLS_MODEL_LOCATION));
#if CONFIG_IDF_TARGET_ESP32P4
    m_image_preprocessor = new dl::image::ImagePreprocessor(
        m_model, {123.675, 116.28, 103.53}, {58.395, 57.12, 57.375}, DL_IMAGE_CAP_RGB565_BIG_ENDIAN);
#else
    m_image_preprocessor =
        new dl::image::ImagePreprocessor(m_model, {123.675, 116.28, 103.53}, {58.395, 57.12, 57.375});
#endif
    m_postprocessor = new dl::cls::ImageNetClsPostprocessor(m_model, top_k, std::numeric_limits<float>::lowest(), true);
}

} // namespace imagenet_cls

ImageNetCls::ImageNetCls(model_type_t model_type, const int top_k)
{
    switch (model_type) {
    case model_type_t::MOBILENETV2_S8_V1:
#if CONFIG_IMAGENET_CLS_MOBILENETV2_S8_V1
        m_model = new imagenet_cls::MobileNetV2("imagenet_cls_mobilenetv2_s8_v1.espdl", top_k);
#else
        ESP_LOGE("imagenet_cls", "imagenet_cls_mobilenetv2_s8_v1 is not selected in menuconfig.");
#endif
        break;
    }
}
