#include "dl_base_elemwise.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {

int calculate_elemwise_stride(std::vector<int> shape, int dim)
{
    int offset = 0;
    if (shape[dim] > 1) {
        if (shape[dim + 1] > 1) {
            // case1: (n, n)
            return offset;
        } else {
            // case1: (n, 1)
            offset = 1;
            for (int i = dim + 1; i < shape.size(); i++) {
                offset *= shape[i];
            }
            return offset;
        }
    } else {
        if (shape[dim + 1] > 1) {
            // case1: (1, n)
            offset = -1;
            for (int i = dim + 1; i < shape.size(); i++) {
                offset *= shape[i];
            }
            return offset;
        } else {
            // case1: (1, 1)
            return offset;
        }
    }
    return offset;
}

template <typename feature_t>
std::vector<elemwiseArgsType<feature_t>> get_elemwise_operation_args(TensorBase *output,
                                                                     TensorBase *input0,
                                                                     TensorBase *input1,
                                                                     const runtime_mode_t runtime_mode)
{
    elemwiseArgsType<feature_t> args;

    // Align the shape of input1 and input0 with output
    std::vector<int> output_shape = output->get_shape();
    int dims = output_shape.size();
    std::vector<int> input0_shape = input0->get_shape();
    std::vector<int> input1_shape = input1->get_shape();
    int input0_dims = input0_shape.size();
    int input1_dims = input1_shape.size();

    if (input0_dims < dims) {
        input0_shape.insert(input0_shape.begin(), dims - input0_dims, 1);
    }
    if (input1_dims < dims) {
        input1_shape.insert(input1_shape.begin(), dims - input1_dims, 1);
    }

    // Merge input0 and input1 shape
    // case1: (m,n) + (m,n) -> (m*n) + (m*n)
    // case2: (m,1) + (n,1) -> (m*n) + (1)
    // case3: (1,m) + (1,n) -> (1) + (m*n)
    int merged_dims = dims;
    for (int i = 0; i < dims; i++) {
        int j = i + 1;
        for (; j < dims; j++) {
            if (input0_shape[i] == input1_shape[i] && input0_shape[j] == input1_shape[j]) {
                input0_shape[i] *= input0_shape[j];
                input1_shape[i] *= input1_shape[j];
                output_shape[i] *= output_shape[j];
                input0_shape[j] = 1;
                input1_shape[j] = 1;
                output_shape[j] = 1;
            } else if (input0_shape[i] == 1 && input0_shape[j] == 1) {
                input1_shape[i] *= input1_shape[j];
                output_shape[i] *= output_shape[j];
                input1_shape[j] = 1;
                output_shape[j] = 1;
            } else if (input1_shape[i] == 1 && input1_shape[j] == 1) {
                input0_shape[i] *= input0_shape[j];
                output_shape[i] *= output_shape[j];
                input0_shape[j] = 1;
                output_shape[j] = 1;
            } else {
                break;
            }
        }
        if (j == dims) {
            merged_dims = i + 1;
            break;
        }
    }

    // printf("merged shape:%d\n", merged_dims);
    // printf("shape: %s, shape:%s shape:%s\n",
    //        shape_to_string(input0_shape).c_str(),
    //        shape_to_string(input1_shape).c_str(),
    //        shape_to_string(output_shape).c_str());

    // Assign args
    // Note: d0 is the last dimension of the input/output tensor.
    // It is convenient to support the higher dimensions in the future.
    assert(merged_dims <= 4); // only support 4D element-wise op
    args.output_element = output->get_element_ptr<feature_t>();
    args.input0_element = input0->get_element_ptr<feature_t>();
    args.input1_element = input1->get_element_ptr<feature_t>();
    switch (merged_dims) {
    case 1:
        args.dims = 1;
        args.output_d0 = output_shape[0];

        args.input0_d0 = input0_shape[0];
        args.input1_d0 = input1_shape[0];
        break;
    case 2:
        args.dims = 2;
        args.output_d0 = output_shape[1];
        args.output_d1 = output_shape[0];

        args.input0_d0 = input0_shape[1];
        args.input1_d0 = input1_shape[1];
        args.input0_d1_stride = input0_shape[0] == 1 ? 0 : input0_shape[1];
        args.input1_d1_stride = input1_shape[0] == 1 ? 0 : input1_shape[1];
        break;
    case 3:
        args.dims = 3;
        args.output_d0 = output_shape[2];
        args.output_d1 = output_shape[1];
        args.output_d2 = output_shape[0];

        args.input0_d0 = input0_shape[2];
        args.input1_d0 = input1_shape[2];
        args.input0_d1_stride = input0_shape[1] == 1 ? 0 : input0_shape[2];
        args.input1_d1_stride = input1_shape[1] == 1 ? 0 : input1_shape[2];
        args.input0_d2_stride = calculate_elemwise_stride(input0_shape, 0);
        args.input1_d2_stride = calculate_elemwise_stride(input1_shape, 0);
        break;
    case 4:
        args.dims = 4;
        args.output_d0 = output_shape[3];
        args.output_d1 = output_shape[2];
        args.output_d2 = output_shape[1];
        args.output_d3 = output_shape[0];

        args.input0_d0 = input0_shape[3];
        args.input1_d0 = input1_shape[3];
        args.input0_d1_stride = input0_shape[2] == 1 ? 0 : input0_shape[3];
        args.input1_d1_stride = input1_shape[2] == 1 ? 0 : input1_shape[3];
        args.input0_d2_stride = calculate_elemwise_stride(input0_shape, 1);
        args.input1_d2_stride = calculate_elemwise_stride(input1_shape, 1);
        args.input0_d3_stride = calculate_elemwise_stride(input0_shape, 0);
        args.input1_d3_stride = calculate_elemwise_stride(input1_shape, 0);
        break;
    default:
        ESP_LOGE("Element-wise", "Do not support dim=%d", merged_dims);
        break;
    }

    // for ISA
    int u = 16 / sizeof(feature_t);
    int c_div_x = args.output_d0 / u;
    args.c_remainder = (args.output_d0 % u) * sizeof(feature_t);
    args.c_div_x_1 = c_div_x - 1;
    args.c_div_2x_1 = DL_MAX(c_div_x / 2 - 1, 0);
    args.c_left_x_1 = c_div_x - 2 * args.c_div_2x_1 - 1;

    args.mul_shift = output->exponent - input0->exponent - input1->exponent;

    args.input0_scale = DL_SCALE(input0->exponent);
    args.input1_scale = DL_SCALE(input1->exponent);
    args.output_rescale = DL_RESCALE(output->exponent);
    // args.mul_shift = DL_MAX(args.mul_shift, 0); //

    // todo:: support two core
    std::vector<elemwiseArgsType<feature_t>> m_args(1, args);
    return m_args;
}
template std::vector<elemwiseArgsType<int8_t>> get_elemwise_operation_args(TensorBase *output,
                                                                           TensorBase *input0,
                                                                           TensorBase *input1,
                                                                           const runtime_mode_t runtime_mode);
template std::vector<elemwiseArgsType<int16_t>> get_elemwise_operation_args(TensorBase *output,
                                                                            TensorBase *input0,
                                                                            TensorBase *input1,
                                                                            const runtime_mode_t runtime_mode);

// 4D loop for element-wise op
template <typename feature_t>
void elemwise_loop_4d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func)
{
    feature_t *output_element = args->output_element;
    feature_t *input0_element = args->input0_element;
    feature_t *input1_element = args->input1_element;
    int output_d0 = args->output_d0;
    int input0_d1_stride = args->input0_d1_stride;
    int input1_d1_stride = args->input1_d1_stride;
    int input0_d2_stride = args->input0_d2_stride;
    int input1_d2_stride = args->input1_d2_stride;
    int input0_d3_stride = args->input0_d3_stride;
    int input1_d3_stride = args->input1_d3_stride;

    for (int d3 = 0; d3 < args->output_d3; d3++) {
        for (int d2 = 0; d2 < args->output_d2; d2++) {
            for (int d1 = 0; d1 < args->output_d1; d1++) {
                elemwise_func(output_element, input0_element, input1_element, args);
                // for (int d0 = 1; d0 < output_d0; d0++) {
                //     printf("output_element[%d] = %d\n", d0, output_element[d0]);
                // }
                output_element += args->output_d0;
                input0_element += args->input0_d1_stride;
                input1_element += args->input1_d1_stride;
            }
            input0_element += args->input0_d2_stride;
            input1_element += args->input1_d2_stride;
        }
        input0_element += input0_d3_stride;
        input1_element += input1_d3_stride;
    }
}
template void elemwise_loop_4d(
    elemwiseArgsType<int8_t> *args,
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func);
template void elemwise_loop_4d(
    elemwiseArgsType<int16_t> *args,
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func);

// 3D loop for element-wise op
template <typename feature_t>
void elemwise_loop_3d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func)
{
    feature_t *output_element = args->output_element;
    feature_t *input0_element = args->input0_element;
    feature_t *input1_element = args->input1_element;
    int output_d0 = args->output_d0;
    int input0_d1_stride = args->input0_d1_stride;
    int input1_d1_stride = args->input1_d1_stride;
    int input0_d2_stride = args->input0_d2_stride;
    int input1_d2_stride = args->input1_d2_stride;

    for (int d2 = 0; d2 < args->output_d2; d2++) {
        for (int d1 = 0; d1 < args->output_d1; d1++) {
            elemwise_func(output_element, input0_element, input1_element, args);
            // for (int d0 = 1; d0 < output_d0; d0++) {
            //     printf("output_element[%d] = %d\n", d0, output_element[d0]);
            // }
            output_element += output_d0;
            input0_element += input0_d1_stride;
            input1_element += input1_d1_stride;
        }
        input0_element += input0_d2_stride;
        input1_element += input1_d2_stride;
    }
}
template void elemwise_loop_3d(
    elemwiseArgsType<int8_t> *args,
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func);
template void elemwise_loop_3d(
    elemwiseArgsType<int16_t> *args,
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func);

// 2D loop for element-wise op
template <typename feature_t>
void elemwise_loop_2d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func)
{
    feature_t *output_element = args->output_element;
    feature_t *input0_element = args->input0_element;
    feature_t *input1_element = args->input1_element;
    int output_d0 = args->output_d0;
    int input0_d1_stride = args->input0_d1_stride;
    int input1_d1_stride = args->input1_d1_stride;

    for (int d1 = 0; d1 < args->output_d1; d1++) {
        elemwise_func(output_element, input0_element, input1_element, args);
        // for (int d0 = 1; d0 < output_d0; d0++) {
        //     printf("output_element[%d] = %d\n", d0, output_element[d0]);
        // }
        output_element += output_d0;
        input0_element += input0_d1_stride;
        input1_element += input1_d1_stride;
    }
}
template void elemwise_loop_2d(
    elemwiseArgsType<int8_t> *args,
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func);
template void elemwise_loop_2d(
    elemwiseArgsType<int16_t> *args,
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func);

// 1D loop for element-wise op
template <typename feature_t>
void elemwise_loop_1d(
    elemwiseArgsType<feature_t> *args,
    std::function<void(feature_t *, feature_t *, feature_t *, elemwiseArgsType<feature_t> *)> elemwise_func)
{
    feature_t *output_element = args->output_element;
    feature_t *input0_element = args->input0_element;
    feature_t *input1_element = args->input1_element;
    elemwise_func(output_element, input0_element, input1_element, args);
}
template void elemwise_loop_1d(
    elemwiseArgsType<int8_t> *args,
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func);
template void elemwise_loop_1d(
    elemwiseArgsType<int16_t> *args,
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func);

} // namespace base
} // namespace dl
