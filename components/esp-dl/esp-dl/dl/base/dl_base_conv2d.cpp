#include "dl_base_conv2d.hpp"

#include "dl_base_activate_buffer.hpp"
#include "dl_base_activate_output.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
inline void conv2d_11cn(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    const feature_t *filter_element = (const feature_t *)args.filter_element;

    // filter in sequence [H, W, C, N]
    // for (size_t input_c = 0; input_c < args.input_channel; input_c++)
    // {
    //     for (size_t output_c = 0; output_c < args.output_channel; output_c++)
    //     {
    //         buffer_ptr[output_c] += input_ptr[input_c] * (*filter_element++);
    //     }
    // }

    // filter in sequence [N, H, W, C]
    for (size_t output_c = 0; output_c < args.output_channel; output_c++) // N
    {
        buffer_t acc = 0;
        for (size_t input_c = 0; input_c < args.input_channel; input_c++) // C
        {
            acc += input_ptr[input_c] * (*filter_element++);
        }
        buffer_ptr[output_c] = acc;
    }
}

template <typename feature_t, typename buffer_t>
inline void conv2d_33cn(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    // filter in sequence [H, W, C, N]
    // const filter_t *filter_r0 = args.filter_element;
    // const filter_t *filter_r1 = filter_r0 + args.filter_y_offset;
    // const filter_t *filter_r2 = filter_r1 + args.filter_y_offset;
    // feature_t *&input_syx_d0 = input_ptr;

    // for (size_t filter_x = 0; filter_x < 3; filter_x++)                           // W
    // {                                                                             //
    //     feature_t *input_syx_d1 = input_syx_d0 + args.input_dilation_y_offset;      //
    //     feature_t *input_syx_d2 = input_syx_d1 + args.input_dilation_y_offset;      //
    //     for (size_t input_c = 0; input_c < args.input_channel; input_c++)         // C
    //     {                                                                         //
    //         for (size_t output_c = 0; output_c < args.output_channel; output_c++) // N
    //         {
    //             buffer_ptr[output_c] += input_syx_d0[input_c] * (*filter_r0);
    //             buffer_ptr[output_c] += input_syx_d1[input_c] * (*filter_r1);
    //             buffer_ptr[output_c] += input_syx_d2[input_c] * (*filter_r2);

    //             filter_r0++;
    //             filter_r1++;
    //             filter_r2++;
    //         }
    //     }
    //     input_syx_d0 += args.input_dilation_x_offset;
    // }

    // filter in sequence [N, H, W, C]
    feature_t *input_00 = input_ptr;
    feature_t *input_01 = input_00 + args.input_dilation_x_offset;
    feature_t *input_02 = input_01 + args.input_dilation_x_offset;

    feature_t *input_10 = input_00 + args.input_dilation_y_offset;
    feature_t *input_11 = input_10 + args.input_dilation_x_offset;
    feature_t *input_12 = input_11 + args.input_dilation_x_offset;

    feature_t *input_20 = input_10 + args.input_dilation_y_offset;
    feature_t *input_21 = input_20 + args.input_dilation_x_offset;
    feature_t *input_22 = input_21 + args.input_dilation_x_offset;

    const feature_t *filter_00 = (const feature_t *)args.filter_element;
    const feature_t *filter_01 = filter_00 + args.input_channel;
    const feature_t *filter_02 = filter_01 + args.input_channel;

    const feature_t *filter_10 = filter_00 + args.filter_y_offset_c;
    const feature_t *filter_11 = filter_10 + args.input_channel;
    const feature_t *filter_12 = filter_11 + args.input_channel;

    const feature_t *filter_20 = filter_10 + args.filter_y_offset_c;
    const feature_t *filter_21 = filter_20 + args.input_channel;
    const feature_t *filter_22 = filter_21 + args.input_channel;

    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        buffer_t acc = 0;
        for (size_t input_c = 0; input_c < args.input_channel; input_c++) {
            acc += input_00[input_c] * filter_00[input_c];
            acc += input_01[input_c] * filter_01[input_c];
            acc += input_02[input_c] * filter_02[input_c];
            acc += input_10[input_c] * filter_10[input_c];
            acc += input_11[input_c] * filter_11[input_c];
            acc += input_12[input_c] * filter_12[input_c];
            acc += input_20[input_c] * filter_20[input_c];
            acc += input_21[input_c] * filter_21[input_c];
            acc += input_22[input_c] * filter_22[input_c];
        }
        filter_00 += args.filter_n_offset_c;
        filter_01 += args.filter_n_offset_c;
        filter_02 += args.filter_n_offset_c;
        filter_10 += args.filter_n_offset_c;
        filter_11 += args.filter_n_offset_c;
        filter_12 += args.filter_n_offset_c;
        filter_20 += args.filter_n_offset_c;
        filter_21 += args.filter_n_offset_c;
        filter_22 += args.filter_n_offset_c;

        buffer_ptr[output_c] = acc;
    }
}

template <typename feature_t, typename buffer_t>
inline void conv2d_hwcn(buffer_t *buffer_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    // filter in sequence [H, W, C, N]
    // const filter_t *filter_element = args.filter_element;                             // Reload filter
    // feature_t *&input_syx_dy = input_ptr;                                               //
    // for (size_t filter_y = 0; filter_y < args.filter_height; filter_y++)              // H
    // {                                                                                 //
    //     feature_t *input_syx_dyx = input_syx_dy;                                        //
    //     for (size_t filter_x = 0; filter_x < args.filter_width; filter_x++)           // W
    //     {                                                                             //
    //         for (size_t input_c = 0; input_c < args.input_channel; input_c++)         // C
    //         {                                                                         //
    //             for (size_t output_c = 0; output_c < args.output_channel; output_c++) // N
    //             {
    //                 buffer_ptr[output_c] += input_syx_dyx[input_c] * (*filter_element);
    //                 filter_element++;
    //             }
    //         }
    //         input_syx_dyx += args.input_dilation_x_offset;
    //     }
    //     input_syx_dy += args.input_dilation_y_offset;
    // }

    // filter in sequence [N, H, W, C]
    const feature_t *filter_element = (const feature_t *)args.filter_element;
    for (size_t output_c = 0; output_c < args.output_channel; output_c++)         // N
    {                                                                             //
        feature_t *input_syx_dy = input_ptr;                                      //
        buffer_t acc = 0;                                                         //
        for (size_t filter_y = 0; filter_y < args.filter_height; filter_y++)      // H
        {                                                                         //
            feature_t *input_syx_dyx = input_syx_dy;                              //
            for (size_t filter_x = 0; filter_x < args.filter_width; filter_x++)   // W
            {                                                                     //
                for (size_t input_c = 0; input_c < args.input_channel; input_c++) // C
                {
                    acc += input_syx_dyx[input_c] * (*filter_element++);
                }
                input_syx_dyx += args.input_dilation_x_offset;
            }
            filter_element += args.filter_y_offset;
            input_syx_dy += args.input_dilation_y_offset;
        }
        filter_element += args.filter_n_offset;
        buffer_ptr[output_c] = acc;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize conv2d<int16_t, int16_t, DL_S16_BUFFER_TYPE>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_conv2d_11cn_s16(i_impl_func_s16_t &i_impl_func,
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
                i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_11cn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_bias_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_11cn_prelu;
                break;
            }
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_11cn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_11cn_prelu;
                break;
            }
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#elif CONFIG_XTENSA_BOOST
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn_bias;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn_bias_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn_bias;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn_bias;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_11cn;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#else // C/C++ implementation
    c_impl_func_sp = conv2d_11cn<int16_t, DL_S16_BUFFER_TYPE>;
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
    return;
#endif
}

inline void load_conv2d_33cn_s16(i_impl_func_s16_t &i_impl_func,
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
                i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_bias;
                i_impl_func = dl_esp32p4_s16_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_bias_relu;
                i_impl_func = dl_esp32p4_s16_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_bias_relu;
                // i_impl_func = dl_esp32p4_s16_conv2d_hwcn_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_bias_prelu;
                // i_impl_func = dl_esp32p4_s16_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn;
                i_impl_func = dl_esp32p4_s16_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_relu;
                i_impl_func = dl_esp32p4_s16_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_relu;
                // i_impl_func = dl_esp32p4_s16_conv2d_hwcn_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_33cn_prelu;
                // i_impl_func = dl_esp32p4_s16_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_bias;
                i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_bias_relu;
                i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_bias_leakyrelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_bias_prelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn;
                i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_relu;
                i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_leakyrelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_33cn_prelu;
                // i_impl_func = dl_esp32p4_s16_unaligned_conv2d_hwcn_prelu;
                break;
            }
        }
    }
    return;

#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_bias;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_bias_relu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_bias_relu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_bias_prelu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn;
                i_impl_func = dl_tie728_s16_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_relu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_relu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_33cn_prelu;
                i_impl_func = dl_tie728_s16_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_bias;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_bias_relu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_bias_prelu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_relu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_leakyrelu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_33cn_prelu;
                i_impl_func = dl_tie728_s16_unaligned_conv2d_hwcn_prelu;
                break;
            }
        }
    }
    return;

#elif CONFIG_XTENSA_BOOST
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn_bias;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn_bias;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn_bias_relu;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn_bias_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn_bias;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn_bias;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn_bias;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn_bias;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn_relu;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_33cn;
            i_impl_func = dl_xtensa_s16_conv2d_hwcn;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    }
    return;

#else // C/C++ implementation
    c_impl_func_sp = conv2d_33cn<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
    c_impl_func = conv2d_hwcn<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
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
    return;
#endif
}

inline void load_conv2d_hwcn_s16(i_impl_func_s16_t &i_impl_func,
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
                i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_bias_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_relu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s16_unaligned_conv2d_hwcn_prelu;
                break;
            }
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#elif CONFIG_TIE728_BOOST
    if (args.output_channel % 8 == 0 && args.input_channel % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_bias_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_relu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case PReLU:
                i_impl_func_sp = dl_tie728_s16_unaligned_conv2d_hwcn_prelu;
                break;
            }
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#elif CONFIG_XTENSA_BOOST
    if (args.bias_element) {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn_bias;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn_bias_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn_bias;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn_bias;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn;
            break;
        case ReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn_relu;
            break;
        case LeakyReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn;
            n_wise_func = output_leakyrelu<int16_t, DL_S16_BUFFER_TYPE>;
            break;
        case PReLU:
            i_impl_func_sp = dl_xtensa_s16_conv2d_hwcn;
            n_wise_func = output_prelu<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
            break;
        }
    }
    i_impl_func = i_impl_func_sp;
    return;

#else // C/C++ implement
    c_impl_func_sp = conv2d_hwcn<int16_t, int16_t, DL_S16_BUFFER_TYPE>;
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
    return;
#endif
}

template <>
void conv2d<int16_t, int32_t, int64_t>(void *args_ptr)
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

    if (args.filter_height == 1 && args.filter_width == 1) // Filter shape = [1, 1, C, N]
    {
        load_conv2d_11cn_s16(i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func, args);
    } else if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        load_conv2d_33cn_s16(i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func, args);
    } else // Filter shape = [H, W, C, N]
    {
        load_conv2d_hwcn_s16(i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func, args);
    }

    conv_operation_shell<int16_t, int64_t>(args, i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize conv2d<int8_t, int8_t, int32_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_conv2d_11cn_s8(i_impl_func_s8_t &i_impl_func,
                                i_impl_func_s8_t &i_impl_func_sp,
                                const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_11cn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_11cn_prelu;
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
            i_impl_func_sp = dl_tie728_s8_conv2d_11cn;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_11cn_relu;
            break;
        case PReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_11cn_prelu;
            break;
        }
    } else {
        i_impl_func_sp = dl_tie728_s8_unaligned_conv2d_11cn;
    }
    i_impl_func = i_impl_func_sp;
    return;
#endif
}

inline void load_conv2d_33cn_s8(i_impl_func_s8_t &i_impl_func,
                                i_impl_func_s8_t &i_impl_func_sp,
                                const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn_bias;
                i_impl_func = dl_esp32p4_s8_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn_bias_relu;
                i_impl_func = dl_esp32p4_s8_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn_bias_prelu;
                // i_impl_func = dl_esp32p4_s8_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn;
                i_impl_func = dl_esp32p4_s8_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn_relu;
                i_impl_func = dl_esp32p4_s8_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_33cn_prelu;
                // i_impl_func = dl_esp32p4_s8_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn_bias;
                i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu;
                i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn_bias_prelu;
                // i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn;
                i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn_leakyrelu;
                i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_33cn_prelu;
                // i_impl_func = dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu;
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
            i_impl_func_sp = dl_tie728_s8_conv2d_33cn;
            i_impl_func = dl_tie728_s8_conv2d_hwcn;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_33cn_relu;
            i_impl_func = dl_tie728_s8_conv2d_hwcn_relu;
            break;
        case PReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_33cn_prelu;
            i_impl_func = dl_tie728_s8_conv2d_hwcn_prelu;
            break;
        }
    } else {
        i_impl_func_sp = dl_tie728_s8_unaligned_conv2d_33cn;
        i_impl_func = dl_tie728_s8_unaligned_conv2d_hwcn;
    }
    return;
#endif
}

inline void load_conv2d_hwcn_s8(i_impl_func_s8_t &i_impl_func,
                                i_impl_func_s8_t &i_impl_func_sp,
                                const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.output_channel % 16 == 0 && args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn_bias_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn_relu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_conv2d_hwcn_prelu;
                break;
            }
        }
    } else {
        if (args.bias_element) {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu;
                break;
            }
        } else {
            switch (args.activation_type) {
            case Linear:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn;
                break;
            case ReLU:
                i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu;
                break;
            case LeakyReLU:
                // Don't be supported now.
                break;
            case PReLU:
                // i_impl_func_sp = dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu;
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
            i_impl_func_sp = dl_tie728_s8_conv2d_hwcn;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_hwcn_relu;
            break;
        case PReLU:
            i_impl_func_sp = dl_tie728_s8_conv2d_hwcn_prelu;
            break;
        }
    } else {
        i_impl_func_sp = dl_tie728_s8_unaligned_conv2d_hwcn;
    }
    i_impl_func = i_impl_func_sp;
    return;
#endif
}

inline void load_conv2d_s8_per_tensor_c_func(c_impl_func_s8_t &c_impl_func,
                                             c_impl_func_s8_t &c_impl_func_sp,
                                             n_wise_func_s8_t &n_wise_func,
                                             const ArgsType<int8_t> &args)
{
    if (args.filter_height == 1 && args.filter_width == 1) // Filter shape = [1, 1, C, N]
    {
        c_impl_func_sp = conv2d_11cn<int8_t, int32_t>;
        c_impl_func = c_impl_func_sp;
    } else if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        c_impl_func_sp = conv2d_33cn<int8_t, int32_t>;
        c_impl_func = conv2d_hwcn<int8_t, int32_t>;
    } else // Filter shape = [H, W, C, N]
    {
        c_impl_func_sp = conv2d_hwcn<int8_t, int32_t>;
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

inline void load_conv2d_s8_per_channel_c_func(c_impl_func_s8_t &c_impl_func,
                                              c_impl_func_s8_t &c_impl_func_sp,
                                              n_wise_func_s8_t &n_wise_func,
                                              const ArgsType<int8_t> &args)
{
    if (args.filter_height == 1 && args.filter_width == 1) // Filter shape = [1, 1, C, N]
    {
        c_impl_func_sp = conv2d_11cn<int8_t, int32_t>;
        c_impl_func = c_impl_func_sp;
    } else if (args.filter_height == 3 && args.filter_width == 3) // Filter shape = [3, 3, C, N]
    {
        c_impl_func_sp = conv2d_33cn<int8_t, int32_t>;
        c_impl_func = conv2d_hwcn<int8_t, int32_t>;
    } else // Filter shape = [H, W, C, N]
    {
        c_impl_func_sp = conv2d_hwcn<int8_t, int32_t>;
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

template <>
void conv2d<int8_t, int32_t, int32_t>(void *args_ptr)
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

    if (args.filter_height == 1 && args.filter_width == 1) {
        load_conv2d_11cn_s8(i_impl_func, i_impl_func_sp, args); // Filter shape = [1, 1, C, N]
    } else if (args.filter_height == 3 && args.filter_width == 3) {
        load_conv2d_33cn_s8(i_impl_func, i_impl_func_sp, args); // Filter shape = [3, 3, C, N]
    } else {
        load_conv2d_hwcn_s8(i_impl_func, i_impl_func_sp, args); // Filter shape = [H, W, C, N]
    }

    if (!i_impl_func || !i_impl_func_sp) {
        load_conv2d_s8_per_channel_c_func(c_impl_func, c_impl_func_sp, n_wise_func, args);
    }

    conv_operation_shell<int8_t, int32_t>(args, i_impl_func, i_impl_func_sp, c_impl_func, c_impl_func_sp, n_wise_func);
}
} // namespace base
} // namespace dl
