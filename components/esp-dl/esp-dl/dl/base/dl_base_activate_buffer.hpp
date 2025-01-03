#pragma once

#include "dl_base.hpp"
#include "dl_define.hpp"
#include "dl_tool.hpp"
#include <stdint.h>

namespace dl {
namespace base {
template <typename feature_t, typename bias_t, typename buffer_t>
inline void buffer_bias_linear(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    bias_t *bias_ptr = (bias_t *)args.bias_element;
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], 4);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c] - 4);

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename bias_t, typename buffer_t>
inline void buffer_bias_relu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    bias_t *bias_ptr = (bias_t *)args.bias_element;
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], 4);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c] - 4);
            // Activation
            if (buffer_ptr[output_c] < 0)
                buffer_ptr[output_c] = 0;

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // Activation
            if (buffer_ptr[output_c] < 0)
                buffer_ptr[output_c] = 0;

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename bias_t, typename buffer_t>
inline void buffer_bias_leakyrelu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    bias_t *bias_ptr = (bias_t *)args.bias_element;
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], 4);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c] - 4);
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= args.activation_alpha;
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= args.activation_alpha;
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename bias_t, typename buffer_t>
inline void buffer_bias_prelu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    bias_t *bias_ptr = (bias_t *)args.bias_element;
    feature_t *alpha_ptr = (feature_t *)args.activation_alpha_ptr;
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], 4);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c] - 4);

            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= alpha_ptr[output_c];
                buffer_ptr[output_c] >>= args.activation_shift;
            }
            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);

            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Bias
            buffer_ptr[output_c] += bias_ptr[output_c];
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= alpha_ptr[output_c];
                buffer_ptr[output_c] >>= args.activation_shift;
            }
            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);

            buffer_ptr[output_c] = 0;
        }
    }
}

/**
 * @brief without bias
 *
 */
template <typename feature_t, typename buffer_t>
inline void buffer_0000_linear(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c]);

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename buffer_t>
inline void buffer_0000_relu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c]);
            // Activation
            if (buffer_ptr[output_c] < 0)
                buffer_ptr[output_c] = 0;

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Activation
            if (buffer_ptr[output_c] < 0)
                buffer_ptr[output_c] = 0;

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename buffer_t>
inline void buffer_0000_leakyrelu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c]);
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= args.activation_alpha;
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= args.activation_alpha;
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}

template <typename feature_t, typename buffer_t>
inline void buffer_0000_prelu(feature_t *output_ptr, buffer_t *buffer_ptr, const ArgsType<feature_t> &args)
{
    feature_t *alpha_ptr = (feature_t *)args.activation_alpha_ptr;
    if (args.mac_shift == INT_MIN) // per-channel
    {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.filter_channel_factor[output_c]);
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= alpha_ptr[output_c];
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    } else {
        for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
            // right shift
            buffer_ptr[output_c] = DL_RIGHT_SHIFT(buffer_ptr[output_c], args.mac_shift);
            // Activation
            if (buffer_ptr[output_c] < 0) {
                buffer_ptr[output_c] *= alpha_ptr[output_c];
                buffer_ptr[output_c] >>= args.activation_shift;
            }

            tool::truncate(output_ptr[output_c], buffer_ptr[output_c]);
            buffer_ptr[output_c] = 0;
        }
    }
}
} // namespace base
} // namespace dl
