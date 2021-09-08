#include "app_common.hpp"

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"


void draw_detection_result(uint16_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results)
{
    int i = 0;
    for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
    {
        dl::image::draw_hollow_rectangle(image_ptr, image_height, image_width,
                                         DL_MAX(prediction->box[0], 0),
                                         DL_MAX(prediction->box[1], 0),
                                         DL_MAX(prediction->box[2], 0),
                                         DL_MAX(prediction->box[3], 0),
                                         COLOR_GREEN);
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[0], 0), DL_MAX(prediction->keypoint[1], 0), 4, COLOR_RED);   // left eye
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[2], 0), DL_MAX(prediction->keypoint[3], 0), 4, COLOR_RED);   // mouth left corner
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[4], 0), DL_MAX(prediction->keypoint[5], 0), 4, COLOR_GREEN); // nose
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[6], 0), DL_MAX(prediction->keypoint[7], 0), 4, COLOR_BLUE);  // right eye
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[8], 0), DL_MAX(prediction->keypoint[9], 0), 4, COLOR_BLUE);  // mouth right corner
#endif
    }
}

void draw_detection_result(uint8_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results)
{
    int i = 0;
    for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
    {
        dl::image::draw_hollow_rectangle(image_ptr, image_height, image_width,
                                         DL_MAX(prediction->box[0], 0),
                                         DL_MAX(prediction->box[1], 0),
                                         DL_MAX(prediction->box[2], 0),
                                         DL_MAX(prediction->box[3], 0),
                                         COLOR_GREEN);
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[0], 0), DL_MAX(prediction->keypoint[1], 0), 4, COLOR_RED);   // left eye
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[2], 0), DL_MAX(prediction->keypoint[3], 0), 4, COLOR_RED);   // mouth left corner
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[4], 0), DL_MAX(prediction->keypoint[5], 0), 4, COLOR_GREEN); // nose
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[6], 0), DL_MAX(prediction->keypoint[7], 0), 4, COLOR_BLUE);  // right eye
        dl::image::draw_point(image_ptr, image_height, image_width, DL_MAX(prediction->keypoint[8], 0), DL_MAX(prediction->keypoint[9], 0), 4, COLOR_BLUE);  // mouth right corner
#endif
    }
}

void print_detection_result(std::list<dl::detect::result_t> &results)
{
    int i = 0;
    for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
    {
        ESP_LOGI("detection_result", "[%d]: (%3d, %3d, %3d, %3d)"
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
                                     " | left eye: (%3d, %3d), right eye: (%3d, %3d), nose: (%3d, %3d), mouth left: (%3d, %3d), mouth right: (%3d, %3d)"
#endif
                 ,
                 i,
                 prediction->box[0], prediction->box[1], prediction->box[2], prediction->box[3]
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
                 ,
                 prediction->keypoint[0], prediction->keypoint[1], // left eye
                 prediction->keypoint[6], prediction->keypoint[7], // right eye
                 prediction->keypoint[4], prediction->keypoint[5], // nose
                 prediction->keypoint[2], prediction->keypoint[3], // mouth left corner
                 prediction->keypoint[8], prediction->keypoint[9]
#endif
        );
    }
}

void *app_camera_decode(camera_fb_t *fb)
{
    if (fb->format == PIXFORMAT_RGB565)
    {
        return (void *)fb->buf;
    }
    else
    {
        uint8_t *image_ptr = (uint8_t *)malloc(fb->height * fb->width * 3 * sizeof(uint8_t));
        if (image_ptr)
        {
            if (fmt2rgb888(fb->buf, fb->len, fb->format, image_ptr))
            {
                return (void *)image_ptr;
            }
            else
            {
                ESP_LOGE(TAG, "fmt2rgb888 failed");
                dl::tool::free_aligned(image_ptr);
            }
        }
        else
        {
            ESP_LOGE(TAG, "malloc memory for image rgb888 failed");
        }
    }
    return NULL;
}