/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#pragma once

#include "array.hpp"
#include "array_real.hpp"
#include "array_imaginary.hpp"
#include "hog.hpp"
#include "sdkconfig.h"

// #define CONFIG_KCF_TEMPLATE_SIZE 40
// #define CONFIG_KCF_ROT_DILATION 400

// #define CONFIG_KCF_SCALE_MULTI false
// #define CONFIG_KCF_SCALE_FIXED true

// #define CONFIG_KCF_FEATURE_HOG true
// #define CONFIG_KCF_FEATURE_HOG_CELL_SIZE 4
// #define CONFIG_KCF_FEATURE_HOG_SECTOR_NUMBER 9 

// #define CONFIG_KCF_FEATURE_GRAY true

// #define CONFIG_KCF_LINEAR_CORRELATION true
// #define CONFIG_KCF_GAUSSIAN_CORRELATION true

// NOTE:
//      range:{0, 1, 2}
//          0: print nothing
//          1: print primary latency
//          2: print more details' latency
// #define CONFIG_KCF_LATENCY_PRINT_LEVEL 1

#define KCF_PRINT_PROGRESS false

#ifdef __cplusplus
extern "C"
{
#endif

#include "kiss_fftndr.h"
#include "esp_log.h"

    void rgb_to_gray_with_hanning_window(cpx *dst_pixel, float *src_pixel, float *weight);

#ifdef __cplusplus
}
#endif

/**
 * @brief 
 * 
 * @tparam T Float or Double, bring into correspondence with kiss_fft
 */
template <class T>
class KCF
{
protected:
    const char *TAG = "KCF/DCF";

private:
    ArrayImaginary<3> template_feature;                               // the template feature, shape = (channel, height, width)
    ArrayImaginary<3> template_feature_in_fft;                        // the template feature in fft, shape = (channel, height, width)
    ArrayImaginary<3> roi_feature;                                    // the RoI feature, shape = (channel, height, width)
    T template_quadratic_sum;                                         // template element quadratic sum
    ArrayReal<T, T, 2> hanning_window;                                // hanning window
    int template_image_shape[3];                                      // template shape (channel, height, width)
    int template_feature_shape[3];                                    // template shape (channel, height, width)
    int template_feature_height_width[2];                             // (height, width)
    ArrayImaginary<2> gaussian_in_fft;                                // gaussian distribution in fft
    ArrayImaginary<2> alpha;                                          // gaussian_in_fft ./ template_in_fft
    T roi_dilation_coefficient = CONFIG_KCF_ROT_DILATION / 100.0;     // The size of RoI on image equals to the size of RoT multiplied dilation coefficient
    T lambda = 0.0001;                                                // regularization
    T output_sigma_factor = 0.125;                                    // bandwidth of gaussian _target
    T update_weight;                                                  // linear interpolation factor for adaptation
    T sigma;                                                          // gaussian kernel bandwidth
    int cell_size;                                                    // HOG cell size
    T scale_roi_vs_template;                                          // scale = roi / template
    struct rectangle_side_t<T> rot;                                   // region of target. Notice: RoI is wider than RoT
    kiss_fftnd_cfg fftnd_cfg;                                         // FFT configuration
    kiss_fftnd_cfg ifftnd_cfg;                                        // IFFT configuration
    int margin = 6;                                                   //
#if CONFIG_KCF_SCALE_MULTI                                            //
    T pyramid_scale = CONFIG_KCF_PYRAMID_SCALE / 100.0;               // scale step for multi-scale estimation
    T larger_scale_weight = CONFIG_KCF_LARGER_SCALE_WEIGHT / 100.0;   // to downweight detection scores of other scales for added stability
    T smaller_scale_weight = CONFIG_KCF_SMALLER_SCALE_WEIGHT / 100.0; // to downweight detection scores of other scales for added stability
#endif                                                                //
#if CONFIG_KCF_FEATURE_HOG                                            //
    HOG<T, T> hog_maker;                                              // HOG Maker
#endif                                                                //

    /**
     * @brief Initialize template image shape, feature shape
     * 
     */
    void init_template_shape();

    /**
     * @brief Initialize a Hanning Window
     * 
     */
    void init_hanning_window();

    /**
     * @brief Initialize a Gaussian Distribution in FFT
     * 
     */
    void init_gaussian_in_fft();

    /**
     * @brief 
     * 
     * @param image 
     * @param scale_adjust 
     */
    void update_roi_feature(ArrayReal<uint8_t, T, 3> &image, T scale_adjust);

    /**
     * @brief Get the gaussian correlation object
     * 
     * @param roi_feature 
     * @return ArrayImaginary<2> 
     */
    ArrayImaginary<2> kernel_correlation(ArrayImaginary<3> &roi_feature);

    /**
     * @brief Get the gaussian correlation object
     * 
     * @param roi_feature 
     * @param template_feature_in_fft 
     * @return ArrayImaginary<2> 
     */
    ArrayImaginary<2> kernel_correlation(ArrayImaginary<3> &roi_feature, ArrayImaginary<3> &template_feature_in_fft);

    void shift_and_gaussian_correlation(ArrayImaginary<2> &c, ArrayImaginary<2> &k, T sum);

    /**
     * @brief Train the template
     * 
     * @param weight 
     */
    void train(T weight);

    /**
     * @brief Detect the object location
     * 
     * @return point_value_t<T, T> 
     */
    point_value_t<T, T> detect();

    /**
     * @brief 
     * 
     * @param timestamp The timestamp of last time
     * @param name 
     * @return int64_t The timestamp of this time
     */
    int64_t print_time_consumption(int64_t timestamp, char *name);

    /**
     * @brief exp(x)
     * 
     * @param x 
     * @return T 
     */
    T fast_exp(T x);

public:
    /**
     * @brief Construct a new KCF object
     * 
     */
    KCF();

    /**
     * @brief Destroy the KCF object
     * 
     */
    ~KCF();

    /**
     * @brief Initialize a KCF/DCF tracker
     * 
     * @param image 
     * @param x Target left up x
     * @param y Target left up y
     * @param width Target width
     * @param height Target height
     */
    bool init(ArrayReal<uint8_t, T, 3> &image, int x, int y, int width, int height);

    /**
     * @brief 
     * 
     * @param image 
     * @param target 
     */
    bool init(ArrayReal<uint8_t, T, 3> &image, struct rectangle_side_t<int> target);

    /**
     * @brief 
     * 
     * @param image 
     * @param target 
     */
    bool init(ArrayReal<uint8_t, T, 3> &image, struct rectangle_coordinate_t<int> target);

    /**
     * @brief Update template and draw out new position of the target
     * 
     * @param image 
     * @return struct rectangle_side_t<T> 
     */
    struct rectangle_side_t<T> update(ArrayReal<uint8_t, T, 3> &image);
};


