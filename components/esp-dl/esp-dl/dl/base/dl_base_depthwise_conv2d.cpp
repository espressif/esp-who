#include "dl_base_depthwise_conv2d.hpp"

#include "dl_base_activate_buffer.hpp"
#include "dl_base_activate_output.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
inline void depthwise_conv2d_11c1(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    const feature_t *filter_element = (feature_t *)args.filter_element;
    for (size_t input_c = 0; input_c < args.input_channel; input_c++) {
        buffer_ptr[input_c] = input_ptr[input_c] * (*filter_element);
        filter_element++;
    }
}

template <typename feature_t, typename buffer_t>
inline void depthwise_conv2d_33c1(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    // static int flag = 1;
    const feature_t *filter_r0 = (feature_t *)args.filter_element;
    const feature_t *filter_r1 = filter_r0 + args.filter_y_offset_c;
    const feature_t *filter_r2 = filter_r1 + args.filter_y_offset_c;
    feature_t *input_row_0 = input_ptr;

    for (size_t filter_x = 0; filter_x < 3; filter_x++)                      // W
    {                                                                        //
        feature_t *input_row_1 = input_row_0 + args.input_dilation_y_offset; //
        feature_t *input_row_2 = input_row_1 + args.input_dilation_y_offset; //
        for (size_t input_c = 0; input_c < args.input_channel; input_c++)    // C
        {                                                                    //
            buffer_ptr[input_c] += input_row_0[input_c] * filter_r0[input_c];
            buffer_ptr[input_c] += input_row_1[input_c] * filter_r1[input_c];
            buffer_ptr[input_c] += input_row_2[input_c] * filter_r2[input_c];
        }
        filter_r0 += args.input_channel;
        filter_r1 += args.input_channel;
        filter_r2 += args.input_channel;
        input_row_0 += args.input_dilation_x_offset;
    }
    // flag = 0;
}

template <typename feature_t, typename buffer_t>
inline void depthwise_conv2d_hwc1(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    const feature_t *filter_element = (feature_t *)args.filter_element;
    for (size_t filter_y = 0; filter_y < args.filter_height; filter_y++)      // H
    {                                                                         //
        feature_t *input_yx = input_ptr;                                      //
        for (size_t filter_x = 0; filter_x < args.filter_width; filter_x++)   // W
        {                                                                     //
            for (size_t input_c = 0; input_c < args.input_channel; input_c++) // C
            {                                                                 //
                buffer_ptr[input_c] += input_yx[input_c] * (*filter_element);
                filter_element++;
            }
            input_yx += args.input_dilation_x_offset;
        }
        filter_element += args.filter_y_offset;
        input_ptr += args.input_dilation_y_offset;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize depthwise_conv2d<int16_t, int16_t, DL_S16_BUFFER_TYPE>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_depthwise_conv2d_33c1_s16(i_impl_func_s16_t &i_impl_func,
                                           i_impl_func_s16_t &i_impl_func_sp,
                                           c_impl_func_s16_t &c_impl_func,
                                           c_impl_func_s16_t &c_impl_func_sp,
                                           n_wise_func_s16_t &n_wise_func,
                                           const ArgsType<int16_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu;
                // i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_bias_prelu;
                // i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1;
                i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_relu;
                // i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_33c1_prelu;
                // i_impl_func = dl_esp32p4_s16_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu;
                // i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_prelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1;
                i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu;
                // i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_prelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    }
#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_bias_prelu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_33c1_prelu;
                i_impl_func = dl_tie728_s16_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_33c1_prelu;
                i_impl_func = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    }
#else // C/C++ implementation
    c_impl_func_sp = depthwise_conv2d_33c1<int16_t, DL_S16_BUFFER_TYPE>;
    c_impl_func = depthwise_conv2d_hwc1<int16_t, DL_S16_BUFFER_TYPE>;
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_bias_linear<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case ReLU:
            n_wise_func = buffer_bias_relu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_bias_leakyrelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            n_wise_func = buffer_bias_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_0000_linear<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case ReLU:
            n_wise_func = buffer_0000_relu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_0000_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            n_wise_func = buffer_0000_prelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    }
#endif
}

inline void load_depthwise_conv2d_hwc1_s16(i_impl_func_s16_t &i_impl_func,
                                           i_impl_func_s16_t &i_impl_func_sp,
                                           c_impl_func_s16_t &c_impl_func,
                                           c_impl_func_s16_t &c_impl_func_sp,
                                           n_wise_func_s16_t &n_wise_func,
                                           const ArgsType<int16_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    }

    i_impl_func = i_impl_func_sp;
#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    }

    i_impl_func = i_impl_func_sp;
#else // C/C++ implementation
    c_impl_func_sp = depthwise_conv2d_hwc1<int16_t, DL_S16_BUFFER_TYPE>;
    c_impl_func = c_impl_func_sp;
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_bias_linear<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case ReLU:
            n_wise_func = buffer_bias_relu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_bias_leakyrelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            n_wise_func = buffer_bias_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_0000_linear<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case ReLU:
            n_wise_func = buffer_0000_relu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_0000_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            n_wise_func = buffer_0000_prelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    }
#endif
}

template <>
void depthwise_conv2d<int16_t, int32_t, int64_t>(void *args_ptr)
{
    ArgsType<int16_t> &args = *((ArgsType<int16_t> *)args_ptr);

    i_impl_func_s16_t i_impl_func = NULL;
    i_impl_func_s16_t i_impl_func_sp = NULL;
    c_impl_func_s16_t c_impl_func = NULL;
    c_impl_func_s16_t c_impl_func_sp = NULL;
    n_wise_func_s16_t n_wise_func = NULL;

#if CONFIG_ESP32P4_BOOST
    dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
#endif

    if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        load_depthwise_conv2d_33c1_s16(i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func, args);
    } else // Filter shape = [H, W, C, N]
    {
        load_depthwise_conv2d_hwc1_s16(i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func, args);
    }
    dwconv_operation_shell<int16_t, DL_S16_BUFFER_TYPE>(
        args, i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize depthwise_conv2d<int8_t, int8_t, int32_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void load_depthwise_conv2d_33c1_s8(i_impl_func_s8_t &i_impl_func,
                                          i_impl_func_s8_t &i_impl_func_sp,
                                          const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1_bias_prelu;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_33c1_prelu;
                i_impl_func = dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_bias;
                i_impl_func = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_bias_relu;
                i_impl_func = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1;
                i_impl_func = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_33c1_relu;
                i_impl_func = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_prelu;
                break;
            }
        }
    }
    return;
#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_33c1;
            i_impl_func = dl_tie728_s8_depthwise_conv2d_hwc1;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_33c1_relu;
            i_impl_func = dl_tie728_s8_depthwise_conv2d_hwc1_relu;
            break;
        case PReLU:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_33c1_prelu;
            i_impl_func = dl_tie728_s8_depthwise_conv2d_hwc1_prelu;
            break;
        }
    } else {
        i_impl_func_sp = dl_tie728_s8_unaligned_depthwise_conv2d_33c1;
        i_impl_func = dl_tie728_s8_unaligned_depthwise_conv2d_hwc1;
    }
    return;
#endif
}

inline void load_depthwise_conv2d_hwc1_s8(i_impl_func_s8_t &i_impl_func,
                                          i_impl_func_s8_t &i_impl_func_sp,
                                          const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_depthwise_conv2d_hwc1_prelu;
                break;
            }
        }
    }
    i_impl_func = i_impl_func_sp;
    return;
#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_hwc1;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_hwc1_relu;
            break;
        case PReLU:
            i_impl_func_sp = dl_tie728_s8_depthwise_conv2d_hwc1_prelu;
            break;
        }
    } else {
        i_impl_func_sp = dl_tie728_s8_unaligned_depthwise_conv2d_hwc1;
    }
    i_impl_func = i_impl_func_sp;
    return;
#endif
}

inline void load_depthwise_conv2d_s8_per_tensor_c_func(c_impl_func_s8_t &c_impl_func,
                                                       c_impl_func_s8_t &c_impl_func_sp,
                                                       n_wise_func_s8_t &n_wise_func,
                                                       const ArgsType<int8_t> &args)
{
    if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        c_impl_func_sp = depthwise_conv2d_33c1<int8_t, int32_t>;
        c_impl_func = depthwise_conv2d_hwc1<int8_t, int32_t>;
    } else // Filter shape = [H, W, C, N]
    {
        c_impl_func_sp = depthwise_conv2d_hwc1<int8_t, int32_t>;
        c_impl_func = c_impl_func_sp;
    }
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_bias_linear<int8_t, int8_t, int32_t>;
            break;
        case ReLU:
            n_wise_func = buffer_bias_relu<int8_t, int8_t, int32_t>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_bias_leakyrelu<int8_t, int8_t, int32_t>;
            break;
        case PReLU:
            n_wise_func = buffer_bias_prelu<int8_t, int8_t, int32_t>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_0000_linear<int8_t, int32_t>;
            break;
        case ReLU:
            n_wise_func = buffer_0000_relu<int8_t, int32_t>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_0000_leakyrelu<int8_t, int32_t>;
            break;
        case PReLU:
            n_wise_func = buffer_0000_prelu<int8_t, int32_t>;
            break;
        }
    }
}

inline void load_depthwise_conv2d_s8_per_channel_c_func(c_impl_func_s8_t &c_impl_func,
                                                        c_impl_func_s8_t &c_impl_func_sp,
                                                        n_wise_func_s8_t &n_wise_func,
                                                        const ArgsType<int8_t> &args)
{
    if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        c_impl_func_sp = depthwise_conv2d_33c1<int8_t, int32_t>;
        c_impl_func = depthwise_conv2d_hwc1<int8_t, int32_t>;
    } else // Filter shape = [H, W, C, N]
    {
        c_impl_func_sp = depthwise_conv2d_hwc1<int8_t, int32_t>;
        c_impl_func = c_impl_func_sp;
    }
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_bias_linear<int8_t, int16_t, int32_t>;
            break;
        case ReLU:
            n_wise_func = buffer_bias_relu<int8_t, int16_t, int32_t>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_bias_leakyrelu<int8_t, int16_t, int32_t>;
            break;
        case PReLU:
            n_wise_func = buffer_bias_prelu<int8_t, int16_t, int32_t>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            n_wise_func = buffer_0000_linear<int8_t, int32_t>;
            break;
        case ReLU:
            n_wise_func = buffer_0000_relu<int8_t, int32_t>;
            break;
        case LeakyReLU:
            n_wise_func = buffer_0000_leakyrelu<int8_t, int32_t>;
            break;
        case PReLU:
            n_wise_func = buffer_0000_prelu<int8_t, int32_t>;
            break;
        }
    }
}

template <>
void depthwise_conv2d<int8_t, int32_t, int32_t>(void *args_ptr)
{
    ArgsType<int8_t> &args = *((ArgsType<int8_t> *)args_ptr);

    i_impl_func_s8_t i_impl_func = NULL;
    i_impl_func_s8_t i_impl_func_sp = NULL;
    c_impl_func_s8_t c_impl_func = NULL;
    c_impl_func_s8_t c_impl_func_sp = NULL;
    n_wise_func_s8_t n_wise_func = NULL;

#if CONFIG_ESP32P4_BOOST
    dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
#endif

    if (args.filter_height == 3 && args.filter_width == 3) {
        load_depthwise_conv2d_33c1_s8(i_impl_func, i_impl_func_sp, args); // Filter shape = [3, 3, C, N]
    } else {
        load_depthwise_conv2d_hwc1_s8(i_impl_func, i_impl_func_sp, args); // Filter shape = [H, W, C, N]
    }

    if (!i_impl_func || !i_impl_func_sp) {
        load_depthwise_conv2d_s8_per_channel_c_func(c_impl_func, c_impl_func_sp, n_wise_func, args);
    }
    dwconv_operation_shell<int8_t, int32_t>(
        args, i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func);
}
} // namespace base
} // namespace dl
