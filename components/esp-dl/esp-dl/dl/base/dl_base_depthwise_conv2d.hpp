#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {
/**
 * @brief
 * NOTE: support [H, W, C, 1] only by now
 * NOTE: in tensorflow, when dilation > 1 the stride must be 1. Our api has no such limitation. But we didn't test this
 * oppsite situation. https://tensorflow.google.cn/api_docs/python/tf/nn/depthwise_conv2d
 * TODO: support [H, W, C, M] take tensorflow for reference
 *
 * NOTE: support: feature_t == filter_t == bias_t == activate_t == buffer_t == int16_t
 * TODO: support: feature_t == filter_t == bias_t == activate_t == buffer_t == int8_t
 *
 * @tparam feature_t
 * @tparam bias_t
 * @tparam buffer_t
 * @param args_ptr
 */
template <typename feature_t, typename bias_t, typename buffer_t>
void depthwise_conv2d(void *args_ptr);
} // namespace base
} // namespace dl
