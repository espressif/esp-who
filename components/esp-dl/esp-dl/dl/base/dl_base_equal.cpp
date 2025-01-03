#include "dl_base.hpp"
#include "dl_base_elemwise.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {

// input0_ptr:vector, input1_ptr:scalar
template <typename feature_t>
void c_impl_equal_n_1(feature_t *output_ptr,
                      feature_t *input0_ptr,
                      feature_t *input1_ptr,
                      elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;
    int32_t temp = input1_ptr[0];
    for (int i = 0; i < length; i++) {
        output_ptr[i] = input0_ptr[i] == temp;
        // buffer = DL_RIGHT_SHIFT(buffer, args->equal_shift);
        // tool::truncate<int32_t>(output_ptr[i], buffer);
    }
}

// input0_ptr:scalar, input1_ptr:vector
template <typename feature_t>
void c_impl_equal_1_n(feature_t *output_ptr,
                      feature_t *input0_ptr,
                      feature_t *input1_ptr,
                      elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;
    int32_t temp = input0_ptr[0];
    for (int i = 0; i < length; i++) {
        output_ptr[i] = input1_ptr[i] == temp;
        // buffer = DL_RIGHT_SHIFT(buffer, args->equal_shift);
        // tool::truncate<int32_t>(output_ptr[i], buffer);
    }
}

// input0_ptr:vector, input1_ptr:vector
template <typename feature_t>
void c_impl_equal_n_n(feature_t *output_ptr,
                      feature_t *input0_ptr,
                      feature_t *input1_ptr,
                      elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;
    // int32_t temp = 0;
    for (int i = 0; i < length; i++) {
        output_ptr[i] = input0_ptr[i] == input1_ptr[i];
        // buffer = DL_RIGHT_SHIFT(buffer, args->equal_shift);
        // tool::truncate<int32_t>(output_ptr[i], buffer);
    }
}

void elemwise_equal(elemwiseArgsType<int8_t> *args)
{
    int ilen = 16 / sizeof(int8_t);
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func =
        c_impl_equal_n_n<int8_t>; // default impl

    if (args->output_d0 >= ilen) {
#if CONFIG_IDF_TARGET_ESP32P4
        if (args->input0_d0 % ilen == 0 && args->input1_d0 % ilen == 0) {
            elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_16_w2_16_simdequal;
        } else if (args->input1_d0 == 1) {
            if (args->input0_d0 % ilen == 0) {
                elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_16_w2_1_simdequal;
            } else {
                elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_16_w2_1_simdequal_unaligned;
            }
        } else if (args->input0_d0 == 1) {
            if (args->input1_d0 % ilen == 0) {
                elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_1_w2_16_simdequal;
            } else {
                elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_1_w2_16_simdequal_unaligned;
            }
        } else {
            elemwise_func = dl_esp32p4_s8_equal4d_bchw_w1_16_w2_16_simdequal_unaligned;
        }
// #elif CONFIG_IDF_TARGET_ESP32S3
//         if (args->input0_d0 % ilen == 0 && args->input1_d0 % ilen == 0) {
//             elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_16_w2_16_simdequal;
//         } else if (args->input1_d0 == 1) {
//             if (args->input0_d0 % ilen == 0) {
//                 elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_16_w2_1_simdequal;
//             } else {
//                 elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_16_w2_1_simdequal_unaligned;
//             }
//         } else if (args->input0_d0 == 1) {
//             if (args->input1_d0 % ilen == 0) {
//                 elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_1_w2_16_simdequal;
//             } else {
//                 elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_1_w2_16_simdequal_unaligned;
//             }
//         } else {
//             elemwise_func = dl_esp32s3_s8_equal4d_bchw_w1_16_w2_16_simdequal_unaligned;
//         }
#else
        if (args->input1_d0 == 1) {
            elemwise_func = c_impl_equal_n_1<int8_t>;
        } else if (args->input0_d0 == 1) {
            elemwise_func = c_impl_equal_1_n<int8_t>;
        }
#endif
    } else {
        if (args->input1_d0 == 1) {
            elemwise_func = c_impl_equal_n_1<int8_t>;
        } else if (args->input0_d0 == 1) {
            elemwise_func = c_impl_equal_1_n<int8_t>;
        }
    }

    switch (args->dims) {
    case 1:
        elemwise_loop_1d(args, elemwise_func);
        break;
    case 2:
        elemwise_loop_2d(args, elemwise_func);
        break;
    case 3:
        elemwise_loop_3d(args, elemwise_func);
        break;
    case 4:
        elemwise_loop_4d(args, elemwise_func);
        break;
    default:
        break;
    }
}

void elemwise_equal(elemwiseArgsType<int16_t> *args)
{
    int ilen = 16 / sizeof(int16_t);
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func =
        c_impl_equal_n_n<int16_t>;

    if (args->output_d0 >= ilen) {
#if CONFIG_IDF_TARGET_ESP32P4
        if (args->input0_d0 % ilen == 0 && args->input1_d0 % ilen == 0) {
            // printf("use simd dl_esp32p4_s16_equal4d_bchw_w1_8_w2_8_simdequal\n");
            elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_8_w2_8_simdequal;
        } else if (args->input1_d0 == 1) {
            if (args->input0_d0 % ilen == 0) {
                elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_8_w2_1_simdequal;
            } else {
                elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_8_w2_1_simdequal_unaligned;
            }
        } else if (args->input0_d0 == 1) {
            if (args->input1_d0 % ilen == 0) {
                elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_1_w2_8_simdequal;
            } else {
                elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_1_w2_8_simdequal_unaligned;
            }
        } else {
            elemwise_func = dl_esp32p4_s16_equal4d_bchw_w1_8_w2_8_simdequal_unaligned;
        }
// #elif CONFIG_IDF_TARGET_ESP32S3
//         if (args->input0_d0 % ilen == 0 && args->input1_d0 % ilen == 0) {
//             elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_8_w2_8_simdequal;
//         } else if (args->input1_d0 == 1) {
//             if (args->input0_d0 % ilen == 0) {
//                 elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_8_w2_1_simdequal;
//             } else {
//                 elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_8_w2_1_simdequal_unaligned;
//             }
//         } else if (args->input0_d0 == 1) {
//             if (args->input1_d0 % ilen == 0) {
//                 elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_1_w2_8_simdequal;
//             } else {
//                 elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_1_w2_8_simdequal_unaligned;
//             }
//         } else {
//             elemwise_func = dl_esp32s3_s16_equal4d_bchw_w1_8_w2_8_simdequal_unaligned;
//         }
#else
        if (args->input1_d0 == 1) {
            elemwise_func = c_impl_equal_n_1<int16_t>;
        } else if (args->input0_d0 == 1) {
            elemwise_func = c_impl_equal_1_n<int16_t>;
        }
#endif
    } else {
        if (args->input1_d0 == 1) {
            elemwise_func = c_impl_equal_n_1<int16_t>;
        } else if (args->input0_d0 == 1) {
            elemwise_func = c_impl_equal_1_n<int16_t>;
        }
    }

    switch (args->dims) {
    case 1:
        elemwise_loop_1d(args, elemwise_func);
        break;
    case 2:
        elemwise_loop_2d(args, elemwise_func);
        break;
    case 3:
        elemwise_loop_3d(args, elemwise_func);
        break;
    case 4:
        elemwise_loop_4d(args, elemwise_func);
        break;
    default:
        break;
    }
}

} // namespace base
} // namespace dl
