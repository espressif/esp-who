#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {
/**
 * @brief prelu
 *
 * @tparam feature_t
 */
template <typename feature_t>
void prelu(void *const args_ptr);
} // namespace base
} // namespace dl
