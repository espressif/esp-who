#include "dl_base_shape.hpp"
#include "dl_define.hpp"

namespace dl {
namespace base {

/**
In ONNX, a set of tensors are multidirectional broadcastable to the same shape if one of the following is true:

The tensors all have exactly the same shape.
The tensors all have the same number of dimensions and the length of each dimensions is either a common length or 1.
The tensors that have too few dimensions can have their shapes prepended with a dimension of length 1 to satisfy
property 2. For example, the following tensor shapes are supported by multidirectional broadcasting:

shape(A) = (2, 3, 4, 5), shape(B) = (,), i.e. B is a scalar ==> shape(result) = (2, 3, 4, 5)
shape(A) = (2, 3, 4, 5), shape(B) = (5,), ==> shape(result) = (2, 3, 4, 5)
shape(A) = (4, 5), shape(B) = (2, 3, 4, 5), ==> shape(result) = (2, 3, 4, 5)
shape(A) = (1, 4, 5), shape(B) = (2, 3, 1, 1), ==> shape(result) = (2, 3, 4, 5)
shape(A) = (3, 4, 5), shape(B) = (2, 1, 1, 1), ==> shape(result) = (2, 3, 4, 5)
*/
std::vector<int> get_multidirectional_broadcasting_shape(const std::vector<int> &shape1, const std::vector<int> &shape2)
{
    int dim = shape1.size();
    if (dim < shape2.size()) {
        dim = shape2.size();
    }

    std::vector<int> output_shape(dim);
    for (int i = dim - 1; i >= 0; i--) {
        int index1 = -1;
        int index2 = -1;
        int dim1 = 0;
        int dim2 = 0;

        index1 = shape1.size() - (dim - i);
        index2 = shape2.size() - (dim - i);
        if (index1 >= 0)
            dim1 = shape1[index1];
        if (index2 >= 0)
            dim2 = shape2[index2];
        output_shape[i] = DL_MAX(dim1, dim2);
    }

    return output_shape;
}

/**
In ONNX, tensor B is unidirectional broadcastable to tensor A if one of the following is true:

Tensor A and B both have exactly the same shape.
Tensor A and B all have the same number of dimensions and the length of each dimensions is either a common length or B's
length is 1. Tensor B has too few dimensions, and B can have its shapes prepended with a dimension of length 1 to
satisfy property 2. When unidirectional broadcasting happens, the output's shape is the same as the shape of A (i.e.,
the larger shape of two input tensors).

In the following examples, tensor B is unidirectional broadcastable to tensor A:

shape(A) = (2, 3, 4, 5), shape(B) = (,), i.e. B is a scalar ==> shape(result) = (2, 3, 4, 5)
shape(A) = (2, 3, 4, 5), shape(B) = (5,), ==> shape(result) = (2, 3, 4, 5)
shape(A) = (2, 3, 4, 5), shape(B) = (2, 1, 1, 5), ==> shape(result) = (2, 3, 4, 5)
shape(A) = (2, 3, 4, 5), shape(B) = (1, 3, 1, 5), ==> shape(result) = (2, 3, 4, 5)
*/
std::vector<int> get_unidirectional_broadcasting_shape(const std::vector<int> &shape1, const std::vector<int> &shape2)
{
    return std::vector<int>(shape1);
}

std::vector<int> get_slice_shape(const std::vector<int> &shape,
                                 std::vector<int> start,
                                 std::vector<int> end,
                                 std::vector<int> axes,
                                 std::vector<int> step)
{
    std::vector<int> output_shape = std::vector<int>(shape);
    int dim = shape.size();

    for (int i = 0; i < start.size(); i++) {
        int axis = i;
        if (!axes.empty()) {
            axis = (axes[i] + dim) % dim;
        }
        int start_i = start[i] < 0 ? start[i] + shape[axis] : start[i] % (shape[axis] + 1);
        int end_i = end[i] < 0 ? end[i] + shape[axis] : end[i] % (shape[axis] + 1);
        if (start_i >= end_i) {
            assert(false);
            return {};
        } else {
            if (step.empty()) {
                output_shape[axis] = end_i - start_i;
            } else {
                output_shape[axis] = 1 + (end_i - start_i - 1) / step[i];
            }
        }
    }

    return output_shape;
}

} // namespace base
} // namespace dl
