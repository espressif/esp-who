#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {
/**
 * @brief conv2d
 *
 * @tparam feature_t
 * @tparam bias_t
 * @tparam buffer_t
 * @param args_ptr
 */
template <typename feature_t, typename bias_t, typename buffer_t>
void conv2d(void *const args_ptr);
} // namespace base
} // namespace dl
