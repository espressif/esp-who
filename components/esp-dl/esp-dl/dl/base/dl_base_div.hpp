#pragma once

#include "dl_base.hpp"
#include "dl_base_elemwise.hpp"

namespace dl {
namespace base {

// Get element-wise div operation args
template <typename feature_t>
elemwiseArgsType<feature_t> *get_elemwise_div_args(TensorBase *output,
                                                   TensorBase *input0,
                                                   TensorBase *input1,
                                                   const runtime_mode_t runtime_mode);

/**
 * @brief int8 element-wise div, support multidirectional broadcasting from 1D to 4D
 *
 * @param args elemwiseArgsType
 */
void elemwise_div(elemwiseArgsType<int8_t> *args);

/**
 * @brief int16 element-wise div, support multidirectional broadcasting from 1D to 4D
 *
 * @param args elemwiseArgsType
 */
void elemwise_div(elemwiseArgsType<int16_t> *args);

} // namespace base
} // namespace dl
