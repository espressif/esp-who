#include "dl_image_color.hpp"

namespace dl {
namespace image {
template <typename T1, typename T2>
void convert_img_loop(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    int step_src = DL_IMAGE_IS_PIX_TYPE_RGB888(src_img.pix_type) ? 3 : 1;
    int step_dst = DL_IMAGE_IS_PIX_TYPE_RGB888(dst_img.pix_type) ? 3 : 1;

    if (crop_area.empty()) {
        T1 *src_pix_ptr = (T1 *)src_img.data;
        T2 *dst_pix_ptr = (T2 *)dst_img.data;
        pix_t src_pix;
        src_pix.type = src_img.pix_type;
        pix_t dst_pix;
        dst_pix.type = dst_img.pix_type;
        for (int i = 0; i < dst_img.height * dst_img.width; i++) {
            src_pix.data = (void *)src_pix_ptr;
            dst_pix.data = (void *)dst_pix_ptr;
            convert_pixel(src_pix, dst_pix, caps, norm_lut);
            src_pix_ptr += step_src;
            dst_pix_ptr += step_dst;
        }
    } else {
        int src_gap = step_src * src_img.width - step_src * dst_img.width;
        T1 *src_pix_ptr = (T1 *)src_img.data + crop_area[0] * step_src + crop_area[1] * step_src * src_img.width;
        T2 *dst_pix_ptr = (T2 *)dst_img.data;
        pix_t src_pix;
        src_pix.type = src_img.pix_type;
        pix_t dst_pix;
        dst_pix.type = dst_img.pix_type;
        for (int i = 0; i < dst_img.height; i++) {
            for (int j = 0; j < dst_img.width; j++) {
                src_pix.data = (void *)src_pix_ptr;
                dst_pix.data = (void *)dst_pix_ptr;
                convert_pixel(src_pix, dst_pix, caps, norm_lut);
                src_pix_ptr += step_src;
                dst_pix_ptr += step_dst;
            }
            src_pix_ptr += src_gap;
        }
    }
}
template void convert_img_loop<uint16_t, uint16_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint16_t, uint8_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint16_t, int8_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint16_t, int16_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint8_t, uint8_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint8_t, uint16_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint8_t, int8_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
template void convert_img_loop<uint8_t, int16_t>(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);

void convert_img(const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    // TODO if do nothing ,just copy.
    assert(src_img.data);
    assert(dst_img.data);
    assert(src_img.height > 0 && src_img.width > 0);
    if (crop_area.empty()) {
        dst_img.height = src_img.height;
        dst_img.width = src_img.width;
    } else {
        assert(crop_area.size() == 4);
        assert(crop_area[2] > crop_area[0]);
        assert(crop_area[3] > crop_area[1]);
        assert(crop_area[0] < src_img.width && crop_area[0] >= 0);
        assert(crop_area[1] < src_img.height && crop_area[1] >= 0);
        assert(crop_area[2] <= src_img.width && crop_area[2] > 0);
        assert(crop_area[3] <= src_img.height && crop_area[3] > 0);
        dst_img.height = crop_area[3] - crop_area[1];
        dst_img.width = crop_area[2] - crop_area[0];
    }
    if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
        convert_img_loop<uint16_t, uint16_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY)) {
        convert_img_loop<uint16_t, uint8_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888_QINT8) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT8)) {
        convert_img_loop<uint16_t, int8_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888_QINT16) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT16)) {
        convert_img_loop<uint16_t, int16_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY)) {
        convert_img_loop<uint8_t, uint8_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
        convert_img_loop<uint8_t, uint16_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT8) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888_QINT8) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT8)) {
        convert_img_loop<uint8_t, int8_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else if ((src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT16) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888 && dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888_QINT16) ||
               (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY && dst_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY_QINT16)) {
        convert_img_loop<uint8_t, int16_t>(src_img, dst_img, caps, norm_lut, crop_area);
    } else {
        ESP_LOGE("dl_image_color",
                 "img conversion between fmt %s and %s is not implemented yet.",
                 pix_type_to_str(src_img.pix_type).c_str(),
                 pix_type_to_str(dst_img.pix_type).c_str());
    }
}

#if CONFIG_IDF_TARGET_ESP32P4
esp_err_t convert_img_ppa(const img_t &src_img,
                          img_t &dst_img,
                          ppa_client_handle_t ppa_handle,
                          void *ppa_buffer,
                          size_t ppa_buffer_size,
                          uint32_t caps,
                          void *norm_lut,
                          const std::vector<int> &crop_area)
{
    // TODO if do nothing ,just copy.
    if (!(caps & DL_IMAGE_CAP_PPA)) {
        return ESP_FAIL;
    }
    // Input only support rgb565/rgb888. If output is not rgb565/rgb888, additional convert is needed, just call
    // convert_img.
    ppa_srm_color_mode_t input_srm_color_mode;
    if (convert_pix_type_to_ppa_srm_fmt(src_img.pix_type, &input_srm_color_mode) == ESP_FAIL) {
        return ESP_FAIL;
    }
    ppa_srm_color_mode_t output_srm_color_mode;
    if (convert_pix_type_to_ppa_srm_fmt(src_img.pix_type, &output_srm_color_mode) == ESP_FAIL) {
        return ESP_FAIL;
    }
    assert(ppa_handle);
    assert(ppa_buffer);
    assert(src_img.data);
    assert(dst_img.data);
    assert(src_img.height > 0 && src_img.width > 0);

    ppa_srm_oper_config_t srm_oper_config;
    memset(&srm_oper_config, 0, sizeof(ppa_srm_oper_config_t));
    if (crop_area.empty()) {
        dst_img.height = src_img.height;
        dst_img.width = src_img.width;
        srm_oper_config.in.block_offset_y = 0;
        srm_oper_config.in.block_offset_x = 0;
        srm_oper_config.in.block_h = src_img.height;
        srm_oper_config.in.block_w = src_img.width;
    } else {
        assert(crop_area.size() == 4);
        assert(crop_area[2] > crop_area[0]);
        assert(crop_area[3] > crop_area[1]);
        assert(crop_area[0] < src_img.width && crop_area[0] >= 0);
        assert(crop_area[1] < src_img.height && crop_area[1] >= 0);
        assert(crop_area[2] <= src_img.width && crop_area[2] > 0);
        assert(crop_area[3] <= src_img.height && crop_area[3] > 0);
        dst_img.height = crop_area[3] - crop_area[1];
        dst_img.width = crop_area[2] - crop_area[0];
        srm_oper_config.in.block_offset_y = crop_area[1];
        srm_oper_config.in.block_offset_x = crop_area[0];
        srm_oper_config.in.block_h = dst_img.height;
        srm_oper_config.in.block_w = dst_img.width;
    }
    srm_oper_config.in.buffer = (const void *)src_img.data;
    srm_oper_config.in.pic_h = src_img.height;
    srm_oper_config.in.pic_w = src_img.width;
    srm_oper_config.in.srm_cm = input_srm_color_mode;
    srm_oper_config.rgb_swap = caps & DL_IMAGE_CAP_RGB_SWAP;
    srm_oper_config.byte_swap = !(caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN);

    srm_oper_config.out.buffer = ppa_buffer;
    srm_oper_config.out.buffer_size = ppa_buffer_size;
    srm_oper_config.out.pic_h = dst_img.height;
    srm_oper_config.out.pic_w = dst_img.width;
    srm_oper_config.out.block_offset_x = 0;
    srm_oper_config.out.block_offset_y = 0;

    srm_oper_config.out.srm_cm =
        (dst_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) ? PPA_SRM_COLOR_MODE_RGB565 : PPA_SRM_COLOR_MODE_RGB888;
    srm_oper_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;

    srm_oper_config.scale_x = 1;
    srm_oper_config.scale_y = 1;
    srm_oper_config.mirror_x = false;
    srm_oper_config.mirror_y = false;
    srm_oper_config.mode = PPA_TRANS_MODE_BLOCKING;

    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_handle, &srm_oper_config));
    // additional memory copy needed, performance tradeoff should be considered.
    tool::copy_memory(dst_img.data, ppa_buffer, get_img_byte_size(dst_img));
    return ESP_OK;
}
#endif

} // namespace image
} // namespace dl
