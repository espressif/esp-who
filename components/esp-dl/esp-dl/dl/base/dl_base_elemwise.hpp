
#pragma once

#include <assert.h>
#include <stdint.h>
#include <vector>

#include "dl_define.hpp"
#include "dl_tensor_base.hpp"

namespace dl {
namespace base {

// For element wise: Add, Sub, Mul etc, currently support from 1D to 4D
// Note: d0 is the last dimension of the input/output tensor. This will facilitate support for higher dimensions in the
// future.
template <typename feature_t>
struct elemwiseArgsType {
    feature_t *input0_element; /*<! 0 */
    feature_t *input1_element; /*<! 1 */
    feature_t *output_element; /*<! 2 */

    int input0_d0;        /*<! 3 */
    int input0_d1_stride; /*<! 4 */
    int input0_d2_stride; /*<! 5 */
    int input0_d3_stride; /*<! 6 */
                          //
    int input1_d0;        /*<! 7 */
    int input1_d1_stride; /*<! 8 */
    int input1_d2_stride; /*<! 9 */
    int input1_d3_stride; /*<! 10 */
                          //
    int output_d0;        /*<! 11 */
    int output_d1;        /*<! 12 */
    int output_d2;        /*<! 13 */
    int output_d3;        /*<! 14 */
                          //
    int dims;             /*<! 15 */
    int c_div_x_1;        /*<! 16 */
    int c_div_2x_1;       /*<! 17 */
    int c_left_x_1;       /*<! 18 */
    int c_remainder;      /*<! 19 */

    int mul_shift;        /*<! 20 */
    float input0_scale;   /*<! 21 */
    float input1_scale;   /*<! 22 */
    float output_rescale; /*<! 23 */
    feature_t *table;     /*<! 24 */
};

// Get element-wise operation args
template <typename feature_t>
std::vector<elemwiseArgsType<feature_t>> get_elemwise_operation_args(
    TensorBase *output, TensorBase *input0, TensorBase *input1, const runtime_mode_t runtime_mode = RUNTIME_MODE_AUTO);

// 4D loop for element-wise op
template <typename feature_t>
void elemwise_loop_4d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func);

// 3D loop for element-wise op
template <typename feature_t>
void elemwise_loop_3d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func);

// 2D loop for element-wise op
template <typename feature_t>
void elemwise_loop_2d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func);

// 1D loop for element-wise op
template <typename feature_t>
void elemwise_loop_1d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func);

} // namespace base
} // namespace dl
