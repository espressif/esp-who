#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {
/**
 * @brief multidirectional broadcasting
 *        refer to https://github.com/onnx/onnx/blob/main/docs/Broadcasting.md
 *
 * @param shape1 Shape of input1
 * @param shape2 Shape of input2
 *
 * @return Shape of output
 */
std::vector<int> get_multidirectional_broadcasting_shape(const std::vector<int> &shape1,
                                                         const std::vector<int> &shape2);

/**
 * @brief unidirectional broadcasting
 *        refer to https://github.com/onnx/onnx/blob/main/docs/Broadcasting.md
 *
 * @param shape1 Shape of input1
 * @param shape2 Shape of input2
 *
 * @return Shape of output
 */
std::vector<int> get_unidirectional_broadcasting_shape(const std::vector<int> &shape1, const std::vector<int> &shape2);

/**
 * @brief Get shape after slice
 *        refer to https://onnx.ai/onnx/operators/onnx__Slice.html
 *
 * @param shape  Shape of input
 * @param start Starting indices of corresponding axis in axes
 * @param end Ending indices (exclusive) of corresponding axis in axes
 * @param axes Axes that starts and ends apply to. Negative value means counting dimensions from the back.
 * @param step Slice step of corresponding axis in axes
 *
 * @return Shape after slice
 */
std::vector<int> get_slice_shape(const std::vector<int> &shape,
                                 std::vector<int> start,
                                 std::vector<int> end,
                                 std::vector<int> axes = {},
                                 std::vector<int> step = {});

} // namespace base
} // namespace dl
