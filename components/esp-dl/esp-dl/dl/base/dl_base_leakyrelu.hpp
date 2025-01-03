#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {
/**
 * @brief leakyrelu
 *
 * @tparam feature_t
 */
template <typename feature_t>
void leakyrelu(void *const args_ptr);
} // namespace base
} // namespace dl
