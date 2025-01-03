#pragma once

#include "dl_define.hpp"
#include "dl_tool.hpp"
#include <stdint.h>

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
inline void output_linear(feature_t *output_element, buffer_t *buffer, const ArgsType<feature_t> &args)
{
}

template <typename feature_t, typename buffer_t>
inline void output_relu(feature_t *output_element, buffer_t *buffer, const ArgsType<feature_t> &args)
{
    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        if (output_element[output_c] < 0)
            output_element[output_c] = 0;
    }
}

template <typename feature_t, typename buffer_t>
inline void output_leakyrelu(feature_t *output_element, buffer_t *buffer, const ArgsType<feature_t> &args)
{
    int temp;
    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        if (output_element[output_c] < 0) {
            temp = DL_RIGHT_SHIFT((buffer_t)output_element[output_c] * (buffer_t)args.activation_alpha,
                                  args.activation_shift);
            tool::truncate(output_element[output_c], temp);
        }
    }
}

template <typename feature_t, typename activate_t, typename buffer_t>
inline void output_prelu(feature_t *output_element, buffer_t *buffer, const ArgsType<feature_t> &args)
{
    int temp;
    activate_t *alpha_ptr = (activate_t *)args.activation_alpha_ptr;
    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        // Activation
        if (output_element[output_c] < 0) {
            temp = DL_RIGHT_SHIFT((buffer_t)output_element[output_c] * (buffer_t)alpha_ptr[output_c],
                                  args.activation_shift);
            tool::truncate(output_element[output_c], temp);
        }
    }
}

template <typename feature_t, typename buffer_t>
inline void arith_output_relu(feature_t *output_element, const arithArgsType<void> &args)
{
    for (size_t output_c = 0; output_c < args.channel; output_c++) {
        if (output_element[output_c] < 0)
            output_element[output_c] = 0;
    }
}

template <typename feature_t, typename buffer_t>
inline void arith_output_leakyrelu(feature_t *output_element, const arithArgsType<void> &args)
{
    int temp;
    for (size_t output_c = 0; output_c < args.channel; output_c++) {
        if (output_element[output_c] < 0) {
            temp = DL_RIGHT_SHIFT((buffer_t)output_element[output_c] * (buffer_t)args.activation_alpha,
                                  args.activation_shift);
            tool::truncate(output_element[output_c], temp);
            output_element[output_c] = (feature_t)temp;
        }
    }
}

template <typename feature_t, typename buffer_t>
inline void arith_output_prelu(feature_t *output_element, const arithArgsType<void> &args)
{
    int temp;
    feature_t *alpha_ptr = (feature_t *)args.activation_alpha_ptr;
    for (size_t output_c = 0; output_c < args.channel; output_c++) {
        // Activation
        if (output_element[output_c] < 0) {
            temp = DL_RIGHT_SHIFT((buffer_t)output_element[output_c] * (buffer_t)alpha_ptr[output_c],
                                  args.activation_shift);
            tool::truncate(output_element[output_c], temp);
            output_element[output_c] = (feature_t)temp;
        }
    }
}
} // namespace base
} // namespace dl
