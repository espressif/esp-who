#include "dl_image_jpeg.hpp"

static const char *TAG = "dl_image_jpeg";
namespace dl {
namespace image {
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
// software decode
esp_err_t sw_decode_jpeg(const jpeg_img_t &jpeg_img,
                         img_t &decoded_img,
                         bool swap_color_bytes,
                         esp_jpeg_image_scale_t scale)
{
    uint32_t outbuf_size = jpeg_img.height * jpeg_img.width * 3;
    uint8_t *outbuf = (uint8_t *)heap_caps_malloc(outbuf_size, MALLOC_CAP_SPIRAM);
    if (!(decoded_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 || decoded_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888)) {
        ESP_LOGE(TAG, "Unsupported img pix format.");
        return ESP_FAIL;
    }
    esp_jpeg_image_format_t out_format;
    if (decoded_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
        out_format = JPEG_IMAGE_FORMAT_RGB888;
        swap_color_bytes = !swap_color_bytes;
    } else {
        out_format = JPEG_IMAGE_FORMAT_RGB565;
    }
    // JPEG decode config
    esp_jpeg_image_cfg_t jpeg_cfg = {.indata = jpeg_img.data,
                                     .indata_size = jpeg_img.data_size,
                                     .outbuf = outbuf,
                                     .outbuf_size = outbuf_size,
                                     .out_format = out_format,
                                     .out_scale = scale,
                                     .flags =
                                         {
                                             .swap_color_bytes = swap_color_bytes,
                                         },
                                     .advanced = {},
                                     .priv = {}};

    esp_jpeg_image_output_t outimg;
    ESP_RETURN_ON_ERROR(esp_jpeg_decode(&jpeg_cfg, &outimg), TAG, "Failed to decode img.");
    assert(outimg.height == jpeg_img.height && outimg.width == jpeg_img.width);
    decoded_img.data = (void *)outbuf;
    decoded_img.height = jpeg_img.height;
    decoded_img.width = jpeg_img.width;
    return ESP_OK;
}
#endif
#if CONFIG_IDF_TARGET_ESP32P4
// hardware decode
esp_err_t hw_decode_jpeg(const jpeg_img_t &jpeg_img, img_t &decoded_img, bool swap_color_bytes)
{
    if (DL_IMAGE_IS_PIX_TYPE_QUANT(decoded_img.pix_type)) {
        ESP_LOGE(TAG, "Can not decode to a quant img.");
        return ESP_FAIL;
    }
    jpeg_decode_picture_info_t header_info;
    ESP_RETURN_ON_ERROR(jpeg_decoder_get_info((const uint8_t *)jpeg_img.data, jpeg_img.data_size, &header_info),
                        TAG,
                        "Failed to get jpeg header info.");
    printf("%d\t%d\t%d\t%d\n",
           JPEG_DOWN_SAMPLING_YUV444,
           JPEG_DOWN_SAMPLING_YUV422,
           JPEG_DOWN_SAMPLING_YUV420,
           JPEG_DOWN_SAMPLING_GRAY);
    printf("%d\n", header_info.sample_method);
    // if (header_info.sample_method == JPEG_DOWN_SAMPLING_GRAY) {
    //     if (decoded_img.pix_type != DL_IMAGE_PIX_TYPE_GRAY) {
    //         ESP_LOGE(TAG,
    //                  "Input jpeg is a gray img, can not decode to fmt %s",
    //                  pix_type_to_str(decoded_img.pix_type).c_str());
    //         return ESP_FAIL;
    //     }
    // } else {
    //     if (decoded_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
    //         ESP_LOGE(TAG,
    //                  "Input jpeg is a color img, can not decode to fmt %s",
    //                  pix_type_to_str(decoded_img.pix_type).c_str());
    //         return ESP_FAIL;
    //     }
    // }
    if (header_info.sample_method == JPEG_DOWN_SAMPLING_YUV422 ||
        header_info.sample_method == JPEG_DOWN_SAMPLING_YUV420) {
        decoded_img.height = DL_IMAGE_ALIGN_UP(header_info.height, 16);
        decoded_img.width = DL_IMAGE_ALIGN_UP(header_info.width, 16);
    } else {
        decoded_img.height = header_info.height;
        decoded_img.width = header_info.width;
    }

    // new engine
    jpeg_decoder_handle_t jpgd_handle;
    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };
    ESP_RETURN_ON_ERROR(
        jpeg_new_decoder_engine(&decode_eng_cfg, &jpgd_handle), TAG, "Failed to create decoder engine.");

    // alloc output buffer
    jpeg_decode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };
    size_t img_byte_size;
    img_byte_size = get_img_byte_size(decoded_img);
    size_t rx_buffer_size = 0;
    uint8_t *rx_buf = (uint8_t *)jpeg_alloc_decoder_mem(img_byte_size, &rx_mem_cfg, &rx_buffer_size);
    if (!rx_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory for jpeg decoder output.");
        return ESP_FAIL;
    }

    // decode
    jpeg_decode_cfg_t decode_cfg = {.output_format = convert_pix_type_to_dec_output_fmt(decoded_img.pix_type),
                                    .rgb_order = swap_color_bytes ? JPEG_DEC_RGB_ELEMENT_ORDER_RGB
                                                                  : JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
                                    .conv_std = JPEG_YUV_RGB_CONV_STD_BT601};
    uint32_t out_size = 0;
    ESP_RETURN_ON_ERROR(
        jpeg_decoder_process(
            jpgd_handle, &decode_cfg, jpeg_img.data, jpeg_img.data_size, rx_buf, rx_buffer_size, &out_size),
        TAG,
        "Failed to run decoder process.");

    // del engine
    ESP_RETURN_ON_ERROR(jpeg_del_decoder_engine(jpgd_handle), TAG, "Failed to delete decoder engine.");

    decoded_img.data = (void *)rx_buf;
    return ESP_OK;
}
// hardware encode
esp_err_t hw_encode_jpeg(const img_t &img,
                         jpeg_img_t &encoded_img,
                         jpeg_down_sampling_type_t down_sample_mode,
                         uint8_t img_quality)
{
    if (DL_IMAGE_IS_PIX_TYPE_QUANT(img.pix_type)) {
        ESP_LOGE(TAG, "Can not encode a quant img.");
        return ESP_FAIL;
    }
    if (get_img_channel(img) == 1) {
        if (down_sample_mode != JPEG_DOWN_SAMPLING_GRAY) {
            ESP_LOGE(TAG, "For gray img, jpeg down sample mode must be gray.");
            return ESP_FAIL;
        }
    } else {
        if (down_sample_mode == JPEG_DOWN_SAMPLING_GRAY) {
            ESP_LOGE(TAG, "For color img, jpeg down sample mode must not be gray.");
            return ESP_FAIL;
        }
    }
    encoded_img.height = img.height;
    encoded_img.width = img.width;

    // new engine
    jpeg_encoder_handle_t jpeg_handle;
    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 70,
    };
    ESP_RETURN_ON_ERROR(
        jpeg_new_encoder_engine(&encode_eng_cfg, &jpeg_handle), TAG, "Failed to create encoder engine.");

    // alloc output buffer
    jpeg_encode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_ENC_ALLOC_OUTPUT_BUFFER,
    };
    size_t rx_buffer_size = 0;
    size_t img_size = get_img_byte_size(img);
    // Assume that compression ratio is 1 to ensure the img is encoded successfully.
    uint8_t *rx_buf = (uint8_t *)jpeg_alloc_encoder_mem(img_size, &rx_mem_cfg, &rx_buffer_size);
    if (!rx_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory for jpeg encoder output.");
        return ESP_FAIL;
    }

    // encode
    jpeg_encode_cfg_t enc_config = {
        .height = (uint32_t)img.height,
        .width = (uint32_t)img.width,
        .src_type = convert_pix_type_to_enc_input_fmt(img.pix_type),
        .sub_sample = down_sample_mode,
        .image_quality = img_quality,
    };
    uint32_t out_size = 0;
    ESP_RETURN_ON_ERROR(
        jpeg_encoder_process(
            jpeg_handle, &enc_config, (const uint8_t *)img.data, img_size, rx_buf, rx_buffer_size, &out_size),
        TAG,
        "Failed to run encoder process.");

    // del engine
    ESP_RETURN_ON_ERROR(jpeg_del_encoder_engine(jpeg_handle), TAG, "Failed to delete encoder engine.");

    encoded_img.data = rx_buf;
    encoded_img.data_size = out_size;
    return ESP_OK;
}

esp_err_t write_jpeg(img_t &img, const char *file_name)
{
    jpeg_img_t jpeg_img;
    ESP_RETURN_ON_ERROR(
        hw_encode_jpeg(
            img, jpeg_img, (get_img_channel(img) == 3) ? JPEG_DOWN_SAMPLING_YUV420 : JPEG_DOWN_SAMPLING_GRAY),
        TAG,
        "Failed to encode img.");
    ESP_RETURN_ON_ERROR(write_jpeg(jpeg_img, file_name), TAG, "Failed to write jpeg img.");
    heap_caps_free(jpeg_img.data);
    return ESP_OK;
}
#endif

esp_err_t write_jpeg(jpeg_img_t &img, const char *file_name)
{
    FILE *f = fopen(file_name, "wb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open %s.", file_name);
        return ESP_FAIL;
    }
    size_t size = fwrite(img.data, img.data_size, 1, f);
    if (size != 1) {
        ESP_LOGE(TAG, "Failed to write img data.");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}
} // namespace image
} // namespace dl
