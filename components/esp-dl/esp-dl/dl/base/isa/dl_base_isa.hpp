#pragma once

extern "C" {
#if CONFIG_XTENSA_BOOST
void dl_xtensa_s16_conv2d_11cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_11cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_11cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_11cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_xtensa_s16_conv2d_33cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_33cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_33cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_33cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_xtensa_s16_conv2d_hwcn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_hwcn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_hwcn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_xtensa_s16_conv2d_hwcn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
#endif

#if CONFIG_TIE728_BOOST
void dl_tie728_s16_conv2d_11cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_11cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_11cn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_11cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_11cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_11cn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_unaligned_conv2d_11cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_11cn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_conv2d_33cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_33cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_33cn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_33cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_33cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_33cn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_unaligned_conv2d_33cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_33cn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_conv2d_hwcn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_hwcn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_hwcn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_hwcn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_hwcn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_conv2d_hwcn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_unaligned_conv2d_hwcn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_depthwise_conv2d_33c1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_33c1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_33c1_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_33c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_33c1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_33c1_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_unaligned_depthwise_conv2d_33c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_33c1_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_depthwise_conv2d_hwc1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_hwc1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_depthwise_conv2d_hwc1_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_max_pool2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_max_pool2d_22c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_max_pool2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_max_pool2d_22c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_avg_pool2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_avg_pool2d_22c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_avg_pool2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_avg_pool2d_22c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_add2d_11c_relu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_add2d_11c_prelu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_rescale_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_rescale_add2d_11c_relu(int16_t *output_ptr,
                                          int16_t *input0_ptr,
                                          int16_t *input1_ptr,
                                          void *args_ptr);
void dl_tie728_s16_rescale_add2d_11c_prelu(int16_t *output_ptr,
                                           int16_t *input0_ptr,
                                           int16_t *input1_ptr,
                                           void *args_ptr);
void dl_tie728_s16_unaligned_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_add2d_11c_relu(int16_t *output_ptr,
                                            int16_t *input0_ptr,
                                            int16_t *input1_ptr,
                                            void *args_ptr);
void dl_tie728_s16_unaligned_add2d_11c_prelu(int16_t *output_ptr,
                                             int16_t *input0_ptr,
                                             int16_t *input1_ptr,
                                             void *args_ptr);

void dl_tie728_s16_sub2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_sub2d_11c_relu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_sub2d_11c_prelu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_rescale_sub2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_rescale_sub2d_11c_relu(int16_t *output_ptr,
                                          int16_t *input0_ptr,
                                          int16_t *input1_ptr,
                                          void *args_ptr);
void dl_tie728_s16_rescale_sub2d_11c_prelu(int16_t *output_ptr,
                                           int16_t *input0_ptr,
                                           int16_t *input1_ptr,
                                           void *args_ptr);
void dl_tie728_s16_unaligned_sub2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_sub2d_11c_relu(int16_t *output_ptr,
                                            int16_t *input0_ptr,
                                            int16_t *input1_ptr,
                                            void *args_ptr);
void dl_tie728_s16_unaligned_sub2d_11c_prelu(int16_t *output_ptr,
                                             int16_t *input0_ptr,
                                             int16_t *input1_ptr,
                                             void *args_ptr);

void dl_tie728_s16_mul2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_mul2d_11c_relu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_mul2d_11c_prelu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_mul2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_mul2d_11c_relu(int16_t *output_ptr,
                                            int16_t *input0_ptr,
                                            int16_t *input1_ptr,
                                            void *args_ptr);
void dl_tie728_s16_unaligned_mul2d_11c_prelu(int16_t *output_ptr,
                                             int16_t *input0_ptr,
                                             int16_t *input1_ptr,
                                             void *args_ptr);

void dl_tie728_s16_max2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_max2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);

void dl_tie728_s16_min2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_min2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);

void dl_tie728_s16_relu_11c(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_relu_11c(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_tie728_s16_prelu_11c(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_tie728_s16_unaligned_prelu_11c(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

/* Int8 API */
void dl_tie728_s8_conv2d_11cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_11cn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_11cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_conv2d_11cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_conv2d_33cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_33cn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_33cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_conv2d_33cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_conv2d_hwcn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_hwcn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_conv2d_hwcn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_conv2d_hwcn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_depthwise_conv2d_33c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_depthwise_conv2d_33c1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_depthwise_conv2d_33c1_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_depthwise_conv2d_33c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_depthwise_conv2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_depthwise_conv2d_hwc1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_depthwise_conv2d_hwc1_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_depthwise_conv2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_max_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_max_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_max_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_max_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_avg_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_avg_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_avg_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_avg_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_add2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_add2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_add2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);

void dl_tie728_s8_sub2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_sub2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_sub2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_sub2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_sub2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_rescale_sub2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_sub2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_sub2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_sub2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);

void dl_tie728_s8_mul2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_mul2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_mul2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_mul2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_mul2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_mul2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);

void dl_tie728_s8_max2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_max2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);

void dl_tie728_s8_min2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_min2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);

void dl_tie728_s8_relu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_relu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_prelu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_prelu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_tie728_s8_resize2d_nearest_2x2_c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
#endif

#if CONFIG_IDF_TARGET_ESP32P4
/* Int16 API */
void dl_esp32p4_s16_conv2d_11cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_11cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_11cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_11cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_unaligned_conv2d_11cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_11cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_11cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_11cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_conv2d_33cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_33cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_33cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_33cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_unaligned_conv2d_33cn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_33cn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_33cn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_33cn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_conv2d_hwcn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_hwcn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_hwcn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_conv2d_hwcn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_unaligned_conv2d_hwcn_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_hwcn(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_conv2d_hwcn_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_depthwise_conv2d_33c1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_33c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_33c1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_depthwise_conv2d_hwc1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_depthwise_conv2d_hwc1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu(int16_t *output_ptr, int16_t *input_ptr, void *args_ptr);

void dl_esp32p4_s16_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_add2d_11c_relu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_add2d_11c_prelu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_rescale_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_rescale_add2d_11c_relu(int16_t *output_ptr,
                                           int16_t *input0_ptr,
                                           int16_t *input1_ptr,
                                           void *args_ptr);
void dl_esp32p4_s16_rescale_add2d_11c_prelu(int16_t *output_ptr,
                                            int16_t *input0_ptr,
                                            int16_t *input1_ptr,
                                            void *args_ptr);
void dl_esp32p4_s16_unaligned_add2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_add2d_11c_relu(int16_t *output_ptr,
                                             int16_t *input0_ptr,
                                             int16_t *input1_ptr,
                                             void *args_ptr);
void dl_esp32p4_s16_unaligned_add2d_11c_prelu(int16_t *output_ptr,
                                              int16_t *input0_ptr,
                                              int16_t *input1_ptr,
                                              void *args_ptr);

void dl_esp32p4_s16_mul2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_mul2d_11c_relu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_mul2d_11c_prelu(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_mul2d_11c(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s16_unaligned_mul2d_11c_relu(int16_t *output_ptr,
                                             int16_t *input0_ptr,
                                             int16_t *input1_ptr,
                                             void *args_ptr);
void dl_esp32p4_s16_unaligned_mul2d_11c_prelu(int16_t *output_ptr,
                                              int16_t *input0_ptr,
                                              int16_t *input1_ptr,
                                              void *args_ptr);

/* Int8 API */
void dl_esp32p4_s8_conv2d_11cn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_11cn_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_11cn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_11cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_11cn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_11cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_unaligned_conv2d_11cn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_11cn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_11cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_11cn_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_11cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_conv2d_33cn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_33cn_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_33cn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_33cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_33cn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_33cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_unaligned_conv2d_33cn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_33cn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_33cn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_33cn_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_33cn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_conv2d_hwcn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_hwcn_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_hwcn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_hwcn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_hwcn_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_conv2d_hwcn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_unaligned_conv2d_hwcn_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_hwcn(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_depthwise_conv2d_33c1_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_33c1_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_33c1_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_33c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_33c1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_33c1_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_depthwise_conv2d_hwc1_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_hwc1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_relu(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_mul2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_mul2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_mul2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_mul2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_mul2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_mul2d_11c_prelu(int8_t *output_ptr,
                                             int8_t *input0_ptr,
                                             int8_t *input1_ptr,
                                             void *args_ptr);

void dl_esp32p4_s8_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_add2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_rescale_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_rescale_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_rescale_add2d_11c_prelu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_add2d_11c(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_add2d_11c_relu(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_add2d_11c_prelu(int8_t *output_ptr,
                                             int8_t *input0_ptr,
                                             int8_t *input1_ptr,
                                             void *args_ptr);

void dl_esp32p4_s8_max_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_max_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_max_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_max_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_avg_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_avg_pool2d_22c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_avg_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_avg_pool2d_hwc1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_resize2d_nearest_2x2_c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_resize2d_nearest_2x2_c1(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_prelu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);
void dl_esp32p4_s8_unaligned_prelu_11c(int8_t *output_ptr, int8_t *input_ptr, void *args_ptr);

void dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_add4d_bchw_w1_8_w2_8_simdadd(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_add4d_bchw_w1_1_w2_8_simdadd(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);

void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_16_simdsub(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_1_simdsub(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_sub4d_bchw_w1_1_w2_16_simdsub(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_8_simdsub(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_1_simdsub(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_1_w2_8_simdsub(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);

void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_add4d_bchw_w1_8_w2_8_simdadd_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
// void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_16_simdsub(int8_t *output_ptr,
//                                                   int8_t *input0_ptr,
//                                                   int8_t *input1_ptr,
//                                                   void *args_ptr);
// void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_1_simdsub(int8_t *output_ptr,
//                                                  int8_t *input0_ptr,
//                                                  int8_t *input1_ptr,
//                                                  void *args_ptr);
// void dl_esp32p4_s8_sub4d_bchw_w1_1_w2_16_simdsub(int8_t *output_ptr,
//                                                  int8_t *input0_ptr,
//                                                  int8_t *input1_ptr,
//                                                  void *args_ptr);
// void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_8_simdsub(int16_t *output_ptr,
//                                                  int16_t *input0_ptr,
//                                                  int16_t *input1_ptr,
//                                                  void *args_ptr);
// void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_1_simdsub(int16_t *output_ptr,
//                                                  int16_t *input0_ptr,
//                                                  int16_t *input1_ptr,
//                                                  void *args_ptr);
// void dl_esp32p4_s16_sub4d_bchw_w1_1_w2_8_simdsub(int16_t *output_ptr,
//                                                  int16_t *input0_ptr,
//                                                  int16_t *input1_ptr,
//                                                  void *args_ptr);
void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_16_simdsub_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_sub4d_bchw_w1_16_w2_1_simdsub_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_sub4d_bchw_w1_1_w2_16_simdsub_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_8_simdsub_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_8_w2_1_simdsub_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_sub4d_bchw_w1_1_w2_8_simdsub_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);

void dl_esp32p4_s8_equal4d_bchw_w1_16_w2_16_simdequal(int8_t *output_ptr,
                                                      int8_t *input0_ptr,
                                                      int8_t *input1_ptr,
                                                      void *args_ptr);
void dl_esp32p4_s8_equal4d_bchw_w1_16_w2_1_simdequal(int8_t *output_ptr,
                                                     int8_t *input0_ptr,
                                                     int8_t *input1_ptr,
                                                     void *args_ptr);
void dl_esp32p4_s8_equal4d_bchw_w1_1_w2_16_simdequal(int8_t *output_ptr,
                                                     int8_t *input0_ptr,
                                                     int8_t *input1_ptr,
                                                     void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_8_w2_8_simdequal(int16_t *output_ptr,
                                                     int16_t *input0_ptr,
                                                     int16_t *input1_ptr,
                                                     void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_8_w2_1_simdequal(int16_t *output_ptr,
                                                     int16_t *input0_ptr,
                                                     int16_t *input1_ptr,
                                                     void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_1_w2_8_simdequal(int16_t *output_ptr,
                                                     int16_t *input0_ptr,
                                                     int16_t *input1_ptr,
                                                     void *args_ptr);
void dl_esp32p4_s8_equal4d_bchw_w1_16_w2_16_simdequal_unaligned(int8_t *output_ptr,
                                                                int8_t *input0_ptr,
                                                                int8_t *input1_ptr,
                                                                void *args_ptr);
void dl_esp32p4_s8_equal4d_bchw_w1_16_w2_1_simdequal_unaligned(int8_t *output_ptr,
                                                               int8_t *input0_ptr,
                                                               int8_t *input1_ptr,
                                                               void *args_ptr);
void dl_esp32p4_s8_equal4d_bchw_w1_1_w2_16_simdequal_unaligned(int8_t *output_ptr,
                                                               int8_t *input0_ptr,
                                                               int8_t *input1_ptr,
                                                               void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_8_w2_8_simdequal_unaligned(int16_t *output_ptr,
                                                               int16_t *input0_ptr,
                                                               int16_t *input1_ptr,
                                                               void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_8_w2_1_simdequal_unaligned(int16_t *output_ptr,
                                                               int16_t *input0_ptr,
                                                               int16_t *input1_ptr,
                                                               void *args_ptr);
void dl_esp32p4_s16_equal4d_bchw_w1_1_w2_8_simdequal_unaligned(int16_t *output_ptr,
                                                               int16_t *input0_ptr,
                                                               int16_t *input1_ptr,
                                                               void *args_ptr);

void dl_esp32p4_s8_greater4d_bchw_w1_16_w2_16_simdgreater(int8_t *output_ptr,
                                                          int8_t *input0_ptr,
                                                          int8_t *input1_ptr,
                                                          void *args_ptr);
void dl_esp32p4_s8_greater4d_bchw_w1_16_w2_1_simdgreater(int8_t *output_ptr,
                                                         int8_t *input0_ptr,
                                                         int8_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s8_greater4d_bchw_w1_1_w2_16_simdgreater(int8_t *output_ptr,
                                                         int8_t *input0_ptr,
                                                         int8_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_8_w2_8_simdgreater(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_8_w2_1_simdgreater(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_1_w2_8_simdgreater(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s8_greater4d_bchw_w1_16_w2_16_simdgreater_unaligned(int8_t *output_ptr,
                                                                    int8_t *input0_ptr,
                                                                    int8_t *input1_ptr,
                                                                    void *args_ptr);
void dl_esp32p4_s8_greater4d_bchw_w1_16_w2_1_simdgreater_unaligned(int8_t *output_ptr,
                                                                   int8_t *input0_ptr,
                                                                   int8_t *input1_ptr,
                                                                   void *args_ptr);
void dl_esp32p4_s8_greater4d_bchw_w1_1_w2_16_simdgreater_unaligned(int8_t *output_ptr,
                                                                   int8_t *input0_ptr,
                                                                   int8_t *input1_ptr,
                                                                   void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_8_w2_8_simdgreater_unaligned(int16_t *output_ptr,
                                                                   int16_t *input0_ptr,
                                                                   int16_t *input1_ptr,
                                                                   void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_8_w2_1_simdgreater_unaligned(int16_t *output_ptr,
                                                                   int16_t *input0_ptr,
                                                                   int16_t *input1_ptr,
                                                                   void *args_ptr);
void dl_esp32p4_s16_greater4d_bchw_w1_1_w2_8_simdgreater_unaligned(int16_t *output_ptr,
                                                                   int16_t *input0_ptr,
                                                                   int16_t *input1_ptr,
                                                                   void *args_ptr);

void dl_esp32p4_s8_less4d_bchw_w1_16_w2_16_simdless(int8_t *output_ptr,
                                                    int8_t *input0_ptr,
                                                    int8_t *input1_ptr,
                                                    void *args_ptr);
void dl_esp32p4_s8_less4d_bchw_w1_16_w2_1_simdless(int8_t *output_ptr,
                                                   int8_t *input0_ptr,
                                                   int8_t *input1_ptr,
                                                   void *args_ptr);
void dl_esp32p4_s8_less4d_bchw_w1_1_w2_16_simdless(int8_t *output_ptr,
                                                   int8_t *input0_ptr,
                                                   int8_t *input1_ptr,
                                                   void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_8_w2_8_simdless(int16_t *output_ptr,
                                                   int16_t *input0_ptr,
                                                   int16_t *input1_ptr,
                                                   void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_8_w2_1_simdless(int16_t *output_ptr,
                                                   int16_t *input0_ptr,
                                                   int16_t *input1_ptr,
                                                   void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_1_w2_8_simdless(int16_t *output_ptr,
                                                   int16_t *input0_ptr,
                                                   int16_t *input1_ptr,
                                                   void *args_ptr);
void dl_esp32p4_s8_less4d_bchw_w1_16_w2_16_simdless_unaligned(int8_t *output_ptr,
                                                              int8_t *input0_ptr,
                                                              int8_t *input1_ptr,
                                                              void *args_ptr);
void dl_esp32p4_s8_less4d_bchw_w1_16_w2_1_simdless_unaligned(int8_t *output_ptr,
                                                             int8_t *input0_ptr,
                                                             int8_t *input1_ptr,
                                                             void *args_ptr);
void dl_esp32p4_s8_less4d_bchw_w1_1_w2_16_simdless_unaligned(int8_t *output_ptr,
                                                             int8_t *input0_ptr,
                                                             int8_t *input1_ptr,
                                                             void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_8_w2_8_simdless_unaligned(int16_t *output_ptr,
                                                             int16_t *input0_ptr,
                                                             int16_t *input1_ptr,
                                                             void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_8_w2_1_simdless_unaligned(int16_t *output_ptr,
                                                             int16_t *input0_ptr,
                                                             int16_t *input1_ptr,
                                                             void *args_ptr);
void dl_esp32p4_s16_less4d_bchw_w1_1_w2_8_simdless_unaligned(int16_t *output_ptr,
                                                             int16_t *input0_ptr,
                                                             int16_t *input1_ptr,
                                                             void *args_ptr);

void dl_esp32p4_s8_lessorequal4d_bchw_w1_16_w2_16_simdlessorequal(int8_t *output_ptr,
                                                                  int8_t *input0_ptr,
                                                                  int8_t *input1_ptr,
                                                                  void *args_ptr);
void dl_esp32p4_s8_lessorequal4d_bchw_w1_16_w2_1_simdlessorequal(int8_t *output_ptr,
                                                                 int8_t *input0_ptr,
                                                                 int8_t *input1_ptr,
                                                                 void *args_ptr);
void dl_esp32p4_s8_lessorequal4d_bchw_w1_1_w2_16_simdlessorequal(int8_t *output_ptr,
                                                                 int8_t *input0_ptr,
                                                                 int8_t *input1_ptr,
                                                                 void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_8_w2_8_simdlessorequal(int16_t *output_ptr,
                                                                 int16_t *input0_ptr,
                                                                 int16_t *input1_ptr,
                                                                 void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_8_w2_1_simdlessorequal(int16_t *output_ptr,
                                                                 int16_t *input0_ptr,
                                                                 int16_t *input1_ptr,
                                                                 void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_1_w2_8_simdlessorequal(int16_t *output_ptr,
                                                                 int16_t *input0_ptr,
                                                                 int16_t *input1_ptr,
                                                                 void *args_ptr);
void dl_esp32p4_s8_lessorequal4d_bchw_w1_16_w2_16_simdlessorequal_unaligned(int8_t *output_ptr,
                                                                            int8_t *input0_ptr,
                                                                            int8_t *input1_ptr,
                                                                            void *args_ptr);
void dl_esp32p4_s8_lessorequal4d_bchw_w1_16_w2_1_simdlessorequal_unaligned(int8_t *output_ptr,
                                                                           int8_t *input0_ptr,
                                                                           int8_t *input1_ptr,
                                                                           void *args_ptr);
void dl_esp32p4_s8_lessorequal4d_bchw_w1_1_w2_16_simdlessorequal_unaligned(int8_t *output_ptr,
                                                                           int8_t *input0_ptr,
                                                                           int8_t *input1_ptr,
                                                                           void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_8_w2_8_simdlessorequal_unaligned(int16_t *output_ptr,
                                                                           int16_t *input0_ptr,
                                                                           int16_t *input1_ptr,
                                                                           void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_8_w2_1_simdlessorequal_unaligned(int16_t *output_ptr,
                                                                           int16_t *input0_ptr,
                                                                           int16_t *input1_ptr,
                                                                           void *args_ptr);
void dl_esp32p4_s16_lessorequal4d_bchw_w1_1_w2_8_simdlessorequal_unaligned(int16_t *output_ptr,
                                                                           int16_t *input0_ptr,
                                                                           int16_t *input1_ptr,
                                                                           void *args_ptr);

void dl_esp32p4_s8_greaterorequal4d_bchw_w1_16_w2_16_simdgreaterorequal(int8_t *output_ptr,
                                                                        int8_t *input0_ptr,
                                                                        int8_t *input1_ptr,
                                                                        void *args_ptr);
void dl_esp32p4_s8_greaterorequal4d_bchw_w1_16_w2_1_simdgreaterorequal(int8_t *output_ptr,
                                                                       int8_t *input0_ptr,
                                                                       int8_t *input1_ptr,
                                                                       void *args_ptr);
void dl_esp32p4_s8_greaterorequal4d_bchw_w1_1_w2_16_simdgreaterorequal(int8_t *output_ptr,
                                                                       int8_t *input0_ptr,
                                                                       int8_t *input1_ptr,
                                                                       void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_8_w2_8_simdgreaterorequal(int16_t *output_ptr,
                                                                       int16_t *input0_ptr,
                                                                       int16_t *input1_ptr,
                                                                       void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_8_w2_1_simdgreaterorequal(int16_t *output_ptr,
                                                                       int16_t *input0_ptr,
                                                                       int16_t *input1_ptr,
                                                                       void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_1_w2_8_simdgreaterorequal(int16_t *output_ptr,
                                                                       int16_t *input0_ptr,
                                                                       int16_t *input1_ptr,
                                                                       void *args_ptr);
void dl_esp32p4_s8_greaterorequal4d_bchw_w1_16_w2_16_simdgreaterorequal_unaligned(int8_t *output_ptr,
                                                                                  int8_t *input0_ptr,
                                                                                  int8_t *input1_ptr,
                                                                                  void *args_ptr);
void dl_esp32p4_s8_greaterorequal4d_bchw_w1_16_w2_1_simdgreaterorequal_unaligned(int8_t *output_ptr,
                                                                                 int8_t *input0_ptr,
                                                                                 int8_t *input1_ptr,
                                                                                 void *args_ptr);
void dl_esp32p4_s8_greaterorequal4d_bchw_w1_1_w2_16_simdgreaterorequal_unaligned(int8_t *output_ptr,
                                                                                 int8_t *input0_ptr,
                                                                                 int8_t *input1_ptr,
                                                                                 void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_8_w2_8_simdgreaterorequal_unaligned(int16_t *output_ptr,
                                                                                 int16_t *input0_ptr,
                                                                                 int16_t *input1_ptr,
                                                                                 void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_8_w2_1_simdgreaterorequal_unaligned(int16_t *output_ptr,
                                                                                 int16_t *input0_ptr,
                                                                                 int16_t *input1_ptr,
                                                                                 void *args_ptr);
void dl_esp32p4_s16_greaterorequal4d_bchw_w1_1_w2_8_simdgreaterorequal_unaligned(int16_t *output_ptr,
                                                                                 int16_t *input0_ptr,
                                                                                 int16_t *input1_ptr,
                                                                                 void *args_ptr);

void dl_esp32p4_s8_and4d_bchw_w1_16_w2_16_simdand(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_and4d_bchw_w1_16_w2_1_simdand(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_and4d_bchw_w1_1_w2_16_simdand(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_8_w2_8_simdand(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_8_w2_1_simdand(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_1_w2_8_simdand(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_and4d_bchw_w1_16_w2_16_simdand_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_and4d_bchw_w1_16_w2_1_simdand_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_and4d_bchw_w1_1_w2_16_simdand_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_8_w2_8_simdand_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_8_w2_1_simdand_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_and4d_bchw_w1_1_w2_8_simdand_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);

void dl_esp32p4_s8_xor4d_bchw_w1_16_w2_16_simdxor(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_xor4d_bchw_w1_16_w2_1_simdxor(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_xor4d_bchw_w1_1_w2_16_simdxor(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_8_w2_8_simdxor(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_8_w2_1_simdxor(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_1_w2_8_simdxor(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_xor4d_bchw_w1_16_w2_16_simdxor_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_xor4d_bchw_w1_16_w2_1_simdxor_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_xor4d_bchw_w1_1_w2_16_simdxor_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_8_w2_8_simdxor_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_8_w2_1_simdxor_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_xor4d_bchw_w1_1_w2_8_simdxor_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);

void dl_esp32p4_s8_or4d_bchw_w1_16_w2_16_simdor(int8_t *output_ptr,
                                                int8_t *input0_ptr,
                                                int8_t *input1_ptr,
                                                void *args_ptr);
void dl_esp32p4_s8_or4d_bchw_w1_16_w2_1_simdor(int8_t *output_ptr,
                                               int8_t *input0_ptr,
                                               int8_t *input1_ptr,
                                               void *args_ptr);
void dl_esp32p4_s8_or4d_bchw_w1_1_w2_16_simdor(int8_t *output_ptr,
                                               int8_t *input0_ptr,
                                               int8_t *input1_ptr,
                                               void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_8_w2_8_simdor(int16_t *output_ptr,
                                               int16_t *input0_ptr,
                                               int16_t *input1_ptr,
                                               void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_8_w2_1_simdor(int16_t *output_ptr,
                                               int16_t *input0_ptr,
                                               int16_t *input1_ptr,
                                               void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_1_w2_8_simdor(int16_t *output_ptr,
                                               int16_t *input0_ptr,
                                               int16_t *input1_ptr,
                                               void *args_ptr);
void dl_esp32p4_s8_or4d_bchw_w1_16_w2_16_simdor_unaligned(int8_t *output_ptr,
                                                          int8_t *input0_ptr,
                                                          int8_t *input1_ptr,
                                                          void *args_ptr);
void dl_esp32p4_s8_or4d_bchw_w1_16_w2_1_simdor_unaligned(int8_t *output_ptr,
                                                         int8_t *input0_ptr,
                                                         int8_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s8_or4d_bchw_w1_1_w2_16_simdor_unaligned(int8_t *output_ptr,
                                                         int8_t *input0_ptr,
                                                         int8_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_8_w2_8_simdor_unaligned(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_8_w2_1_simdor_unaligned(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);
void dl_esp32p4_s16_or4d_bchw_w1_1_w2_8_simdor_unaligned(int16_t *output_ptr,
                                                         int16_t *input0_ptr,
                                                         int16_t *input1_ptr,
                                                         void *args_ptr);

void dl_esp32p4_s8_min4d_bchw_w1_16_w2_16_simdmin(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_min4d_bchw_w1_16_w2_1_simdmin(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_min4d_bchw_w1_1_w2_16_simdmin(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_8_w2_8_simdmin(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_8_w2_1_simdmin(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_1_w2_8_simdmin(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_min4d_bchw_w1_16_w2_16_simdmin_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_min4d_bchw_w1_16_w2_1_simdmin_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_min4d_bchw_w1_1_w2_16_simdmin_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_8_w2_8_simdmin_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_8_w2_1_simdmin_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_min4d_bchw_w1_1_w2_8_simdmin_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);

void dl_esp32p4_s8_max4d_bchw_w1_16_w2_16_simdmax(int8_t *output_ptr,
                                                  int8_t *input0_ptr,
                                                  int8_t *input1_ptr,
                                                  void *args_ptr);
void dl_esp32p4_s8_max4d_bchw_w1_16_w2_1_simdmax(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_max4d_bchw_w1_1_w2_16_simdmax(int8_t *output_ptr,
                                                 int8_t *input0_ptr,
                                                 int8_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_8_w2_8_simdmax(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_8_w2_1_simdmax(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_1_w2_8_simdmax(int16_t *output_ptr,
                                                 int16_t *input0_ptr,
                                                 int16_t *input1_ptr,
                                                 void *args_ptr);
void dl_esp32p4_s8_max4d_bchw_w1_16_w2_16_simdmax_unaligned(int8_t *output_ptr,
                                                            int8_t *input0_ptr,
                                                            int8_t *input1_ptr,
                                                            void *args_ptr);
void dl_esp32p4_s8_max4d_bchw_w1_16_w2_1_simdmax_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s8_max4d_bchw_w1_1_w2_16_simdmax_unaligned(int8_t *output_ptr,
                                                           int8_t *input0_ptr,
                                                           int8_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_8_w2_8_simdmax_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_8_w2_1_simdmax_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);
void dl_esp32p4_s16_max4d_bchw_w1_1_w2_8_simdmax_unaligned(int16_t *output_ptr,
                                                           int16_t *input0_ptr,
                                                           int16_t *input1_ptr,
                                                           void *args_ptr);

#endif
}
