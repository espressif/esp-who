#include "dl_variable.hpp"
#include <algorithm>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

using namespace std;
using namespace dl;

namespace dl {

template <typename T>
Tensor<T> &Tensor<T>::set_shape(const vector<int> shape)
{
    assert(shape.size() > 0);
    this->size = 1;
    for (int i = 0; i < shape.size(); ++i) {
        assert(shape[i] >= 0);
        this->size *= shape[i];
    }
    this->shape = shape;

    std::vector<int> axis_offset(this->shape.size(), 1);
    for (int i = shape.size() - 2; i > -1; --i) {
        axis_offset[i] = axis_offset[i + 1] * this->shape[i + 1];
    }
    this->axis_offset = axis_offset;
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::set_shape(const vector<int> shape);
template Tensor<uint8_t> &Tensor<uint8_t>::set_shape(const vector<int> shape);
template Tensor<int32_t> &Tensor<int32_t>::set_shape(const vector<int> shape);
template Tensor<int16_t> &Tensor<int16_t>::set_shape(const vector<int> shape);
template Tensor<int8_t> &Tensor<int8_t>::set_shape(const vector<int> shape);
template Tensor<float> &Tensor<float>::set_shape(const vector<int> shape);

template <typename T>
Tensor<T> &Tensor<T>::flatten()
{
    this->set_shape({this->get_size()});
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::flatten();
template Tensor<uint8_t> &Tensor<uint8_t>::flatten();
template Tensor<int32_t> &Tensor<int32_t>::flatten();
template Tensor<int16_t> &Tensor<int16_t>::flatten();
template Tensor<int8_t> &Tensor<int8_t>::flatten();
template Tensor<float> &Tensor<float>::flatten();

template <typename T>
Tensor<T> &Tensor<T>::reshape(vector<int> shape)
{
    int size_gt = this->get_size();
    int index = -1;
    for (int i = 0; i < shape.size(); ++i) {
        if (shape[i] == -1) {
            assert(index == -1);
            index = i;
        } else {
            assert(shape[i] > 0);
        }
    }
    int size = 1;
    if (index == -1) {
        for (int i = 0; i < shape.size(); ++i) {
            size *= shape[i];
        }
        assert(size == size_gt);
        this->set_shape(shape);
    } else {
        for (int i = 0; i < shape.size(); ++i) {
            if (shape[i] > 0) {
                size *= shape[i];
            }
        }
        assert((size_gt % size) == 0);
        shape[index] = size_gt / size;
        this->set_shape(shape);
    }
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::reshape(vector<int> shape);
template Tensor<uint8_t> &Tensor<uint8_t>::reshape(vector<int> shape);
template Tensor<int32_t> &Tensor<int32_t>::reshape(vector<int> shape);
template Tensor<int16_t> &Tensor<int16_t>::reshape(vector<int> shape);
template Tensor<int8_t> &Tensor<int8_t>::reshape(vector<int> shape);
template Tensor<float> &Tensor<float>::reshape(vector<int> shape);

template <typename T>
Tensor<T> &Tensor<T>::squeeze(int axis)
{
    vector<int> new_shape = this->shape;
    if (axis == INT32_MAX) {
        auto iter = std::remove(new_shape.begin(), new_shape.end(), 1);
        new_shape.erase(iter, new_shape.end());
    } else {
        if (axis < 0)
            axis = new_shape.size() + axis;
        assert(axis >= 0);
        assert(*(new_shape.begin() + axis) == 1);
        new_shape.erase(new_shape.begin() + axis);
    }
    this->set_shape(new_shape);
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::squeeze(int axis);
template Tensor<uint8_t> &Tensor<uint8_t>::squeeze(int axis);
template Tensor<int32_t> &Tensor<int32_t>::squeeze(int axis);
template Tensor<int16_t> &Tensor<int16_t>::squeeze(int axis);
template Tensor<int8_t> &Tensor<int8_t>::squeeze(int axis);
template Tensor<float> &Tensor<float>::squeeze(int axis);

template <typename T>
Tensor<T> &Tensor<T>::expand_dims(int axis)
{
    if (axis < 0)
        axis = this->shape.size() + axis;
    assert(axis >= 0);
    vector<int> new_shape = this->shape;
    new_shape.insert(new_shape.begin() + axis, 1);
    this->set_shape(new_shape);
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::expand_dims(int axis);
template Tensor<uint8_t> &Tensor<uint8_t>::expand_dims(int axis);
template Tensor<int32_t> &Tensor<int32_t>::expand_dims(int axis);
template Tensor<int16_t> &Tensor<int16_t>::expand_dims(int axis);
template Tensor<int8_t> &Tensor<int8_t>::expand_dims(int axis);
template Tensor<float> &Tensor<float>::expand_dims(int axis);

template <typename T>
Tensor<T> &Tensor<T>::expand_dims(vector<int> axis)
{
    assert(axis.size() > 0);
    int size_new = axis.size() + this->shape.size();
    for (int i = 0; i < axis.size(); i++) {
        if (axis[i] < 0) {
            axis[i] = size_new + axis[i];
        }
        assert((axis[i] >= 0) && (axis[i] < size_new));
    }
    sort(axis.begin(), axis.end());

    for (int i = 1; i < axis.size(); i++) {
        assert(axis[i] != axis[i - 1]);
    }

    vector<int> new_shape(size_new, 1);
    int axis_index = 0;
    int shape_index = 0;
    for (int i = 0; i < size_new; i++) {
        if (axis[axis_index] > i) {
            new_shape[i] = this->shape[shape_index++];
        } else {
            axis_index++;
        }
    }
    this->set_shape(new_shape);
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::expand_dims(vector<int> axis);
template Tensor<uint8_t> &Tensor<uint8_t>::expand_dims(vector<int> axis);
template Tensor<int32_t> &Tensor<int32_t>::expand_dims(vector<int> axis);
template Tensor<int16_t> &Tensor<int16_t>::expand_dims(vector<int> axis);
template Tensor<int8_t> &Tensor<int8_t>::expand_dims(vector<int> axis);
template Tensor<float> &Tensor<float>::expand_dims(vector<int> axis);

template <typename T>
Tensor<T> &Tensor<T>::transpose(vector<int> perm)
{
    if (perm.size() == 0) {
        for (int i = this->shape.size() - 1; i >= 0; i--) {
            perm.push_back(i);
        }
    }
    assert(perm.size() == this->shape.size());
    assert(this->element != NULL);
    int dims = perm.size();
    int size = this->get_size();
    Tensor<T> temp(*this, true);
    temp.set_auto_free(true);

    vector<int> index_old(dims, 0);
    vector<int> new_shape(dims, 0);

    for (int i = 0; i < dims; ++i) {
        if (perm[i] < 0)
            perm[i] = dims + perm[i];
        assert((perm[i] >= 0) && (perm[i] < dims));
        new_shape[i] = this->shape[perm[i]];
    }

    this->axis_offset[dims - 1] = 1;

    for (int i = dims - 2; i > -1; --i) {
        this->axis_offset[i] = this->axis_offset[i + 1] * new_shape[i + 1];
    }

    for (int i = 0; i < size; ++i) {
        int dim_div_value = i;
        int index_new = 0;
        for (int j = dims - 1; j > -1; --j) {
            index_old[j] = dim_div_value % this->shape[j];
            dim_div_value /= this->shape[j];
        }

        for (int j = dims - 1; j > -1; --j) {
            index_new += index_old[perm[j]] * this->axis_offset[j];
        }
        this->element[index_new] = temp.element[i];
    }
    this->shape = new_shape;

    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::transpose(vector<int> perm);
template Tensor<uint8_t> &Tensor<uint8_t>::transpose(vector<int> perm);
template Tensor<int32_t> &Tensor<int32_t>::transpose(vector<int> perm);
template Tensor<int16_t> &Tensor<int16_t>::transpose(vector<int> perm);
template Tensor<int8_t> &Tensor<int8_t>::transpose(vector<int> perm);
template Tensor<float> &Tensor<float>::transpose(vector<int> perm);

template <typename T>
Tensor<T> &Tensor<T>::transpose(Tensor<T> &input, vector<int> perm)
{
    if (perm.size() == 0) {
        for (int i = this->shape.size() - 1; i >= 0; i--) {
            perm.push_back(i);
        }
    }
    assert(perm.size() == input.shape.size());
    assert(this->get_size() == input.get_size());
    this->malloc_element();
    int dims = perm.size();
    int size = input.get_size();

    vector<int> index_old(dims, 0);
    vector<int> new_shape(dims, 0);

    for (int i = 0; i < dims; ++i) {
        if (perm[i] < 0)
            perm[i] = dims + perm[i];
        new_shape[i] = input.shape[perm[i]];
    }

    this->axis_offset[dims - 1] = 1;

    for (int i = dims - 2; i > -1; --i) {
        this->axis_offset[i] = this->axis_offset[i + 1] * new_shape[i + 1];
    }

    for (int i = 0; i < size; ++i) {
        int dim_div_value = i;
        int index_new = 0;
        for (int j = dims - 1; j > -1; --j) {
            index_old[j] = dim_div_value % input.shape[j];
            dim_div_value /= input.shape[j];
        }
        for (int j = dims - 1; j > -1; --j) {
            index_new += index_old[perm[j]] * this->axis_offset[j];
        }
        this->element[index_new] = input.element[i];
    }
    this->shape = new_shape;

    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::transpose(Tensor<uint16_t> &input, vector<int> perm);
template Tensor<uint8_t> &Tensor<uint8_t>::transpose(Tensor<uint8_t> &input, vector<int> perm);
template Tensor<int32_t> &Tensor<int32_t>::transpose(Tensor<int32_t> &input, vector<int> perm);
template Tensor<int16_t> &Tensor<int16_t>::transpose(Tensor<int16_t> &input, vector<int> perm);
template Tensor<int8_t> &Tensor<int8_t>::transpose(Tensor<int8_t> &input, vector<int> perm);
template Tensor<float> &Tensor<float>::transpose(Tensor<float> &input, vector<int> perm);

template <typename T>
void Tensor<T>::print(std::vector<int> axis_index_range, const char *message)
{
    if (axis_index_range.size() == 0) {
        for (int i = 0; i < this->shape.size(); i++) {
            axis_index_range.push_back(0);
            axis_index_range.push_back(this->shape[i]);
        }
    }
    assert(axis_index_range.size() == (2 * this->shape.size()));
    std::vector<int> axis_index(this->shape.size(), 0);
    std::vector<int> max_axis_index(this->shape.size(), 0);
    std::vector<int> min_axis_index(this->shape.size(), 0);

    std::cout << message << " [";
    int last_i = this->shape.size() - 1;
    for (int i = 0; i < last_i; i++) {
        axis_index_range[2 * i] =
            (axis_index_range[2 * i] < 0) ? (this->shape[i] + axis_index_range[2 * i]) : axis_index_range[2 * i];
        axis_index_range[2 * i + 1] = (axis_index_range[2 * i + 1] < 0) ? (this->shape[i] + axis_index_range[2 * i + 1])
                                                                        : axis_index_range[2 * i + 1];
        if (axis_index_range[2 * i + 1] > this->shape[i]) {
            axis_index_range[2 * i + 1] = this->shape[i];
        }
        assert(axis_index_range[2 * i + 1] > axis_index_range[2 * i]);
        axis_index[i] = axis_index_range[2 * i];
        min_axis_index[i] = axis_index_range[2 * i];
        max_axis_index[i] = axis_index_range[2 * i + 1] - 1;
        std::cout << axis_index_range[2 * i] << ":" << axis_index_range[2 * i + 1] << ", ";
    }
    axis_index_range[2 * last_i] = (axis_index_range[2 * last_i] < 0)
        ? (this->shape[last_i] + axis_index_range[2 * last_i])
        : axis_index_range[2 * last_i];
    axis_index_range[2 * last_i + 1] = (axis_index_range[2 * last_i + 1] < 0)
        ? (this->shape[last_i] + axis_index_range[2 * last_i + 1])
        : axis_index_range[2 * last_i + 1];
    if (axis_index_range[2 * last_i + 1] > this->shape[last_i]) {
        axis_index_range[2 * last_i + 1] = this->shape[last_i];
    }
    assert(axis_index_range[2 * last_i + 1] > axis_index_range[2 * last_i]);
    axis_index[last_i] = axis_index_range[2 * last_i];
    min_axis_index[last_i] = axis_index_range[2 * last_i];
    max_axis_index[last_i] = axis_index_range[2 * last_i + 1] - 1;
    std::cout << axis_index_range[2 * last_i] << ":" << axis_index_range[2 * last_i + 1] << "] | ";
    std::cout << "exponent:" << this->exponent << " | " << "dtype:" << this->get_dtype_string() << " | ";
    this->print_shape();

    int end_axis_num = this->shape.size();

    while (1) {
        for (int i = 0; i < (this->shape.size() - end_axis_num); i++) {
            std::cout << " ";
        }
        for (int i = 0; i < end_axis_num; i++) {
            std::cout << "[";
        }
        while (axis_index[last_i] < max_axis_index[last_i]) {
            std::cout << +this->get_element_value(axis_index) << " ";
            axis_index[last_i] += 1;
        }
        std::cout << +this->get_element_value(axis_index);
        axis_index[last_i] += 1;

        end_axis_num = 0;
        for (int i = last_i; i > 0; i--) {
            if (axis_index[i] > max_axis_index[i]) {
                axis_index[i] = min_axis_index[i];
                axis_index[i - 1] += 1;
                end_axis_num += 1;
                std::cout << "]";
            }
        }
        if (axis_index[0] > max_axis_index[0]) {
            std::cout << "]\n";
            break;
        } else {
            for (int i = 0; i < end_axis_num; i++) {
                std::cout << "\n";
            }
        }
    }
}
template void Tensor<uint16_t>::print(std::vector<int> axis_index_range = {}, const char *message = "");
template void Tensor<uint8_t>::print(std::vector<int> axis_index_range = {}, const char *message = "");
template void Tensor<int32_t>::print(std::vector<int> axis_index_range = {}, const char *message = "");
template void Tensor<int16_t>::print(std::vector<int> axis_index_range = {}, const char *message = "");
template void Tensor<int8_t>::print(std::vector<int> axis_index_range = {}, const char *message = "");
template void Tensor<float>::print(std::vector<int> axis_index_range = {}, const char *message = "");

template <typename T>
int Tensor<T>::get_element_index(const std::vector<int> axis_index)
{
    assert(axis_index.size() == this->shape.size());
    int element_index = 0;
    for (int i = 0; i < axis_index.size(); i++) {
        element_index += axis_index[i] * this->axis_offset[i];
    }
    return element_index;
}
template int Tensor<uint16_t>::get_element_index(const std::vector<int> axis_index);
template int Tensor<uint8_t>::get_element_index(const std::vector<int> axis_index);
template int Tensor<int32_t>::get_element_index(const std::vector<int> axis_index);
template int Tensor<int16_t>::get_element_index(const std::vector<int> axis_index);
template int Tensor<int8_t>::get_element_index(const std::vector<int> axis_index);
template int Tensor<float>::get_element_index(const std::vector<int> axis_index);

template <typename T>
Tensor<T> &Tensor<T>::set_value(T value)
{
    assert(this->element != NULL);
    dl::tool::set_value(this->element, value, this->size);
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::set_value(uint16_t value);
template Tensor<uint8_t> &Tensor<uint8_t>::set_value(uint8_t value);
template Tensor<int32_t> &Tensor<int32_t>::set_value(int32_t value);
template Tensor<int16_t> &Tensor<int16_t>::set_value(int16_t value);
template Tensor<int8_t> &Tensor<int8_t>::set_value(int8_t value);
template Tensor<float> &Tensor<float>::set_value(float value);

template <typename T>
Tensor<T> &Tensor<T>::set_value(Tensor<T> &value)
{
    assert(this->element != NULL);
    int dims = this->shape.size();
    assert(value.shape.size() == dims);
    for (int i = 0; i < dims; ++i) {
        assert((value.shape[i] == this->shape[i]) || (value.shape[i] == 1));
    }
    if (this->is_same_shape(value)) // just copy
    {
        tool::copy_memory(this->element, value.element, this->get_size() * sizeof(T));
    } else // copy with broadcast
    {
        int min_offset = 0;
        int min_offset_axis = value.axis_offset[0];
        for (int i = dims - 1; i >= 0; --i) {
            if (value.shape[i] != this->shape[i]) {
                min_offset = value.axis_offset[i];
                min_offset_axis = i;
                break;
            }
        }
        int min_offset_bytes = min_offset * sizeof(T);

        std::vector<int> axis_index(dims, 0);
        std::vector<int> value_index(dims, 0);
        T *value_ptr = NULL;
        T *output_ptr = this->element;
        while (axis_index[0] < this->shape[0]) {
            value_ptr = value.element + value.get_element_index(value_index);
            tool::copy_memory(output_ptr, value_ptr, min_offset_bytes);
            output_ptr += min_offset;
            axis_index[min_offset_axis] += 1;
            for (int i = min_offset_axis; i > 0; --i) {
                if (axis_index[i] == this->shape[i]) {
                    axis_index[i] = 0;
                    axis_index[i - 1] += 1;
                    value_index[i] = 0;
                    if (value.shape[i - 1] > 1) {
                        value_index[i - 1] += 1;
                    }
                } else
                    break;
            }
        }
    }
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::set_value(Tensor<uint16_t> &value);
template Tensor<uint8_t> &Tensor<uint8_t>::set_value(Tensor<uint8_t> &value);
template Tensor<int32_t> &Tensor<int32_t>::set_value(Tensor<int32_t> &value);
template Tensor<int16_t> &Tensor<int16_t>::set_value(Tensor<int16_t> &value);
template Tensor<int8_t> &Tensor<int8_t>::set_value(Tensor<int8_t> &value);
template Tensor<float> &Tensor<float>::set_value(Tensor<float> &value);

template <typename T>
Tensor<T> &Tensor<T>::set_value(std::vector<int> axis_index_range, T value)
{
    assert(this->element != NULL);
    int dims = this->shape.size();
    assert(axis_index_range.size() == (2 * dims));
    std::vector<int> loop_index_lower_bound(dims, 0);
    std::vector<int> loop_index_upper_bound(dims, 0);

    for (int i = 0; i < dims; ++i) {
        loop_index_lower_bound[i] =
            axis_index_range[2 * i] < 0 ? this->shape[i] + axis_index_range[2 * i] : axis_index_range[2 * i];
        loop_index_lower_bound[i] = loop_index_lower_bound[i] < 0 ? 0 : loop_index_lower_bound[i];
        loop_index_upper_bound[i] = axis_index_range[2 * i + 1] < 0 ? this->shape[i] + axis_index_range[2 * i + 1]
                                                                    : axis_index_range[2 * i + 1];
        loop_index_upper_bound[i] =
            loop_index_upper_bound[i] > this->shape[i] ? this->shape[i] : loop_index_upper_bound[i];
        if (loop_index_lower_bound[i] == loop_index_upper_bound[i])
            return *this;
        assert(loop_index_lower_bound[i] < loop_index_upper_bound[i]);
    }
    std::vector<int> loop_index = loop_index_lower_bound;
    T *slice_ptr = NULL;
    int min_offset = loop_index_upper_bound[dims - 1] - loop_index_lower_bound[dims - 1];

    if (dims == 1) {
        slice_ptr = this->element + this->get_element_index(loop_index);
        tool::set_value(slice_ptr, value, min_offset);
        return *this;
    }

    while (loop_index[0] < loop_index_upper_bound[0]) {
        slice_ptr = this->element + this->get_element_index(loop_index);
        tool::set_value(slice_ptr, value, min_offset);

        loop_index[dims - 2] += 1;
        for (int i = dims - 2; i > 0; --i) {
            if (loop_index[i] == loop_index_upper_bound[i]) {
                loop_index[i] = loop_index_lower_bound[i];
                loop_index[i - 1] += 1;
            } else
                break;
        }
    }
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::set_value(std::vector<int> axis_index_range, uint16_t value);
template Tensor<uint8_t> &Tensor<uint8_t>::set_value(std::vector<int> axis_index_range, uint8_t value);
template Tensor<int32_t> &Tensor<int32_t>::set_value(std::vector<int> axis_index_range, int32_t value);
template Tensor<int16_t> &Tensor<int16_t>::set_value(std::vector<int> axis_index_range, int16_t value);
template Tensor<int8_t> &Tensor<int8_t>::set_value(std::vector<int> axis_index_range, int8_t value);
template Tensor<float> &Tensor<float>::set_value(std::vector<int> axis_index_range, float value);

template <typename T>
Tensor<T> &Tensor<T>::set_value(std::vector<int> axis_index_range, Tensor<T> &value)
{
    assert(this->element != NULL);
    int dims = this->shape.size();
    assert(axis_index_range.size() == (2 * dims));
    assert(value.shape.size() == dims);
    std::vector<int> output_shape(dims, 0);
    std::vector<int> loop_index_lower_bound(dims, 0);
    std::vector<int> loop_index_upper_bound(dims, 0);

    for (int i = 0; i < dims; ++i) {
        loop_index_lower_bound[i] =
            axis_index_range[2 * i] < 0 ? this->shape[i] + axis_index_range[2 * i] : axis_index_range[2 * i];
        loop_index_lower_bound[i] = loop_index_lower_bound[i] < 0 ? 0 : loop_index_lower_bound[i];
        loop_index_upper_bound[i] = axis_index_range[2 * i + 1] < 0 ? this->shape[i] + axis_index_range[2 * i + 1]
                                                                    : axis_index_range[2 * i + 1];
        loop_index_upper_bound[i] =
            loop_index_upper_bound[i] > this->shape[i] ? this->shape[i] : loop_index_upper_bound[i];
        if (loop_index_lower_bound[i] == loop_index_upper_bound[i])
            return *this;
        assert(loop_index_lower_bound[i] < loop_index_upper_bound[i]);
        output_shape[i] = loop_index_upper_bound[i] - loop_index_lower_bound[i];
        assert((value.shape[i] == output_shape[i]) || (value.shape[i] == 1));
    }
    std::vector<int> loop_index = loop_index_lower_bound;
    T *slice_ptr = NULL;
    T *value_ptr = value.element;

    if (value.shape == output_shape) // just copy
    {
        int min_offset = loop_index_upper_bound[dims - 1] - loop_index_lower_bound[dims - 1];
        int min_offset_axis = dims - 1;
        for (int i = dims - 1; i >= 0; --i) {
            if (this->shape[i] == output_shape[i]) {
                min_offset = this->axis_offset[i] * this->shape[i];
                min_offset_axis = i;
            } else
                break;
        }
        int min_offset_bytes = min_offset * sizeof(T);

        if (min_offset_axis == 0) {
            slice_ptr = this->element + this->get_element_index(loop_index);
            tool::copy_memory(slice_ptr, value_ptr, min_offset_bytes);
            return *this;
        }

        while (loop_index[0] < loop_index_upper_bound[0]) {
            slice_ptr = this->element + this->get_element_index(loop_index);
            tool::copy_memory(slice_ptr, value_ptr, min_offset_bytes);
            value_ptr += min_offset;

            loop_index[min_offset_axis - 1] += 1;
            for (int i = min_offset_axis - 1; i > 0; --i) {
                if (loop_index[i] == loop_index_upper_bound[i]) {
                    loop_index[i] = loop_index_lower_bound[i];
                    loop_index[i - 1] += 1;
                } else
                    break;
            }
        }
        return *this;
    } else // copy with broadcast
    {
        std::vector<int> value_index(dims, 0);

        int min_offset = 0;
        int min_offset_axis = value.axis_offset[0];
        bool broadcast_axis_flag = false;
        for (int i = dims - 1; i >= 0; --i) {
            if ((value.shape[i] != output_shape[i]) || (this->shape[i] != output_shape[i])) {
                min_offset_axis = i;
                min_offset = value.axis_offset[i];
                if (value.shape[i] != output_shape[i])
                    broadcast_axis_flag = true;
                break;
            }
        }
        int min_offset_bytes = min_offset * sizeof(T);

        if (broadcast_axis_flag) {
            while (loop_index[0] < loop_index_upper_bound[0]) {
                slice_ptr = this->element + this->get_element_index(loop_index);
                value_ptr = value.element + value.get_element_index(value_index);
                tool::copy_memory(slice_ptr, value_ptr, min_offset_bytes);
                loop_index[min_offset_axis] += 1;
                for (int i = min_offset_axis; i > 0; --i) {
                    if (loop_index[i] == loop_index_upper_bound[i]) {
                        loop_index[i] = loop_index_lower_bound[i];
                        loop_index[i - 1] += 1;
                        value_index[i] = 0;
                        if (value.shape[i - 1] > 1) {
                            value_index[i - 1] += 1;
                        }
                    } else
                        break;
                }
            }
            return *this;
        } else {
            while (loop_index[0] < loop_index_upper_bound[0]) {
                slice_ptr = this->element + this->get_element_index(loop_index);
                value_ptr = value.element + value.get_element_index(value_index);
                tool::copy_memory(slice_ptr, value_ptr, min_offset_bytes);
                loop_index[min_offset_axis] += 1;
                value_index[min_offset_axis] += 1;
                for (int i = min_offset_axis; i > 0; --i) {
                    if (loop_index[i] == loop_index_upper_bound[i]) {
                        loop_index[i] = loop_index_lower_bound[i];
                        loop_index[i - 1] += 1;
                        value_index[i] = 0;
                        if (value.shape[i - 1] > 1) {
                            value_index[i - 1] += 1;
                        }
                    } else
                        break;
                }
            }
            return *this;
        }
    }
}
template Tensor<uint16_t> &Tensor<uint16_t>::set_value(std::vector<int> axis_index_range, Tensor<uint16_t> &value);
template Tensor<uint8_t> &Tensor<uint8_t>::set_value(std::vector<int> axis_index_range, Tensor<uint8_t> &value);
template Tensor<int32_t> &Tensor<int32_t>::set_value(std::vector<int> axis_index_range, Tensor<int32_t> &value);
template Tensor<int16_t> &Tensor<int16_t>::set_value(std::vector<int> axis_index_range, Tensor<int16_t> &value);
template Tensor<int8_t> &Tensor<int8_t>::set_value(std::vector<int> axis_index_range, Tensor<int8_t> &value);
template Tensor<float> &Tensor<float>::set_value(std::vector<int> axis_index_range, Tensor<float> &value);

template <typename T>
Tensor<T> Tensor<T>::slice(std::vector<int> axis_index_range)
{
    assert(this->element != NULL);
    int dims = this->shape.size();
    assert(axis_index_range.size() == (2 * dims));
    std::vector<int> output_shape(dims, 0);
    std::vector<int> loop_index_lower_bound(dims, 0);
    std::vector<int> loop_index_upper_bound(dims, 0);

    for (int i = 0; i < dims; ++i) {
        loop_index_lower_bound[i] =
            axis_index_range[2 * i] < 0 ? this->shape[i] + axis_index_range[2 * i] : axis_index_range[2 * i];
        loop_index_lower_bound[i] = loop_index_lower_bound[i] < 0 ? 0 : loop_index_lower_bound[i];
        loop_index_upper_bound[i] = axis_index_range[2 * i + 1] < 0 ? this->shape[i] + axis_index_range[2 * i + 1]
                                                                    : axis_index_range[2 * i + 1];
        loop_index_upper_bound[i] =
            loop_index_upper_bound[i] > this->shape[i] ? this->shape[i] : loop_index_upper_bound[i];
        assert(loop_index_lower_bound[i] < loop_index_upper_bound[i]);
        output_shape[i] = loop_index_upper_bound[i] - loop_index_lower_bound[i];
    }
    std::vector<int> loop_index = loop_index_lower_bound;

    Tensor<T> output;
    output.set_shape(output_shape).set_exponent(this->exponent).malloc_element();
    T *output_ptr = output.element;
    T *slice_ptr = NULL;
    int min_offset = output_shape.back();
    int min_offset_bytes = min_offset * sizeof(T);

    if (dims == 1) {
        slice_ptr = this->element + this->get_element_index(loop_index);
        tool::copy_memory(output_ptr, slice_ptr, min_offset_bytes);
        return output;
    }

    while (loop_index[0] < loop_index_upper_bound[0]) {
        slice_ptr = this->element + this->get_element_index(loop_index);
        tool::copy_memory(output_ptr, slice_ptr, min_offset_bytes);
        output_ptr += min_offset;

        loop_index[dims - 2] += 1;
        for (int i = dims - 2; i > 0; --i) {
            if (loop_index[i] == loop_index_upper_bound[i]) {
                loop_index[i] = loop_index_lower_bound[i];
                loop_index[i - 1] += 1;
            } else
                break;
        }
    }
    return output;
}
template Tensor<uint16_t> Tensor<uint16_t>::slice(std::vector<int> axis_index_range);
template Tensor<uint8_t> Tensor<uint8_t>::slice(std::vector<int> axis_index_range);
template Tensor<int32_t> Tensor<int32_t>::slice(std::vector<int> axis_index_range);
template Tensor<int16_t> Tensor<int16_t>::slice(std::vector<int> axis_index_range);
template Tensor<int8_t> Tensor<int8_t>::slice(std::vector<int> axis_index_range);
template Tensor<float> Tensor<float>::slice(std::vector<int> axis_index_range);

template <typename T>
Tensor<T> &Tensor<T>::reverse(std::vector<int> axis)
{
    if (axis.size() == 0) {
        int loop_num = this->size / 2;
        int max_index = this->size - 1;
        T temp;
        for (int i = 0; i < loop_num; ++i) {
            temp = this->element[i];
            this->element[i] = this->element[max_index - i];
            this->element[max_index - i] = temp;
        }
        return *this;
    }

    int dims = this->shape.size();
    int reverse_num = axis.size();
    for (int i = 0; i < reverse_num; ++i) {
        if (axis[i] < 0)
            axis[i] += dims;
        assert(axis[i] < dims);
    }
    sort(axis.begin(), axis.end());
    vector<int> max_axis_index(reverse_num, 0);
    for (int i = 0; i < reverse_num; ++i) {
        max_axis_index[i] = this->shape[axis[i]] - 1;
    }

    vector<int> loop_index(dims, 0);
    int min_offset_axis = axis.back();
    int min_offset = this->axis_offset[min_offset_axis];
    int min_offset_bytes = min_offset * sizeof(T);
    Tensor<T> temp(*this, true);
    temp.set_auto_free(true);

    T *output_ptr = this->element;
    T *temp_ptr = NULL;
    while (loop_index[0] < this->shape[0]) {
        vector<int> loop_index_tmp = loop_index;
        for (int i = 0; i < reverse_num; ++i) {
            loop_index_tmp[axis[i]] = max_axis_index[i] - loop_index_tmp[axis[i]];
        }
        temp_ptr = temp.element + temp.get_element_index(loop_index_tmp);
        tool::copy_memory(output_ptr, temp_ptr, min_offset_bytes);
        output_ptr += min_offset;
        loop_index[min_offset_axis] += 1;
        for (int i = min_offset_axis; i > 0; --i) {
            if (loop_index[i] == this->shape[i]) {
                loop_index[i] = 0;
                loop_index[i - 1] += 1;
            } else
                break;
        }
    }
    return *this;
}
template Tensor<uint16_t> &Tensor<uint16_t>::reverse(vector<int> axis);
template Tensor<uint8_t> &Tensor<uint8_t>::reverse(vector<int> axis);
template Tensor<int32_t> &Tensor<int32_t>::reverse(vector<int> axis);
template Tensor<int16_t> &Tensor<int16_t>::reverse(vector<int> axis);
template Tensor<int8_t> &Tensor<int8_t>::reverse(vector<int> axis);
template Tensor<float> &Tensor<float>::reverse(vector<int> axis);

template <typename T>
bool Tensor<T>::convert_from(TensorBase *input)
{
    if (input->size <= 0 || input->shape.empty()) {
        return false;
    }
    if (input->dtype == DATA_TYPE_FLOAT) {
        Tensor<float> *tensor = static_cast<Tensor<float> *>(input);
        this->convert_from(*tensor);
    } else if (input->dtype == DATA_TYPE_INT8) {
        Tensor<int8_t> *tensor = static_cast<Tensor<int8_t> *>(input);
        this->convert_from(*tensor);
    } else if (input->dtype == DATA_TYPE_INT16) {
        Tensor<int16_t> *tensor = static_cast<Tensor<int16_t> *>(input);
        this->convert_from(*tensor);
    } else {
        return false;
    }
    return false;
}
template bool Tensor<int8_t>::convert_from(TensorBase *input);
template bool Tensor<int16_t>::convert_from(TensorBase *input);
template bool Tensor<float>::convert_from(TensorBase *input);

template <typename T>
bool Tensor<T>::convert_from(const Tensor<int8_t> &input)
{
    int8_t *in_element = input.element;
    T *element = this->element;
    if (in_element && element) {
        if (this->is_same_shape(input)) {
            if (std::is_same<T, int8_t>::value) { // int8 -> int8
                int exponent = input.exponent - this->exponent;
                if (exponent == 0) {
                    tool::copy_memory(element, in_element, this->get_size() * sizeof(T));
                } else {
                    // TODO:: more effective implementation
                    float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                    for (int i = 0; i < this->get_size(); i++) {
                        element[i] = quantize<int8_t>(dequantize(in_element[i], DL_SCALE(input.exponent)), inv_scale);
                    }
                }
            } else if (std::is_same<T, int16_t>::value) { // int8 -> int16
                int exponent = input.exponent - this->exponent;
                if (exponent == 0) {
                    for (int i = 0; i < this->get_size(); i++) {
                        element[i] = in_element[i];
                    }
                } else {
                    // TODO:: more effective implementation
                    float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                    for (int i = 0; i < this->get_size(); i++) {
                        element[i] = quantize<int16_t>(dequantize(in_element[i], DL_SCALE(input.exponent)), inv_scale);
                    }
                }
            } else if (std::is_same<T, float>::value) { // int8 -> float
                for (int i = 0; i < this->get_size(); i++) {
                    element[i] = dequantize(in_element[i], DL_SCALE(input.exponent));
                }
            } else {
                return false;
            }
        }
        return false;
    } else {
        return false;
    }

    return true;
}
template bool Tensor<int8_t>::convert_from(const Tensor<int8_t> &input);
template bool Tensor<int16_t>::convert_from(const Tensor<int8_t> &input);
template bool Tensor<float>::convert_from(const Tensor<int8_t> &input);

template <typename T>
bool Tensor<T>::convert_from(const Tensor<int16_t> &input)
{
    int16_t *in_element = input.element;
    T *element = this->element;
    if (in_element && element) {
        if (this->is_same_shape(input)) {
            if (std::is_same<T, int8_t>::value) { // int16 -> int8
                // TODO:: more effective implementation
                float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                for (int i = 0; i < this->get_size(); i++) {
                    element[i] = quantize<int8_t>(dequantize(in_element[i], DL_SCALE(input.exponent)), inv_scale);
                }
            } else if (std::is_same<T, int16_t>::value) { // int16 -> int16
                int exponent = input.exponent - this->exponent;
                if (exponent == 0) {
                    tool::copy_memory(element, in_element, this->get_size() * sizeof(T));
                } else {
                    // TODO:: more effective implementation
                    float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                    for (int i = 0; i < this->get_size(); i++) {
                        element[i] = quantize<int16_t>(dequantize(in_element[i], DL_SCALE(input.exponent)), inv_scale);
                    }
                }
            } else if (std::is_same<T, float>::value) { // int16 -> float
                for (int i = 0; i < this->get_size(); i++) {
                    element[i] = dequantize(in_element[i], DL_SCALE(input.exponent));
                }
            } else {
                return false;
            }
        }
        return false;
    } else {
        return false;
    }

    return true;
}
template bool Tensor<int8_t>::convert_from(const Tensor<int16_t> &input);
template bool Tensor<int16_t>::convert_from(const Tensor<int16_t> &input);
template bool Tensor<float>::convert_from(const Tensor<int16_t> &input);

template <typename T>
bool Tensor<T>::convert_from(const Tensor<float> &input)
{
    float *in_element = input.element;
    T *element = this->element;
    if (in_element && element) {
        if (this->is_same_shape(input)) {
            if (std::is_same<T, int8_t>::value) { // float -> int8
                // TODO:: more effective implementation
                float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                for (int i = 0; i < this->get_size(); i++) {
                    element[i] = quantize<int8_t>(in_element[i], inv_scale);
                }
            } else if (std::is_same<T, int16_t>::value) { // float -> int16
                float inv_scale = 1.0 / (DL_SCALE(this->exponent));
                for (int i = 0; i < this->get_size(); i++) {
                    element[i] = quantize<int16_t>(in_element[i], inv_scale);
                }
            } else if (std::is_same<T, float>::value) { // float -> float
                tool::copy_memory(element, in_element, this->get_size() * sizeof(T));
            } else {
                return false;
            }
        }
        return false;
    } else {
        return false;
    }

    return true;
}
template bool Tensor<int8_t>::convert_from(const Tensor<float> &input);
template bool Tensor<int16_t>::convert_from(const Tensor<float> &input);
template bool Tensor<float>::convert_from(const Tensor<float> &input);

// template <typename T>
// Tensor<T> &Tensor<T>::set_padding_value(const vector<int> padding, T value)
// {
//     assert(this->shape.size()); // call set_shape() first
//     assert(this->element != NULL);
//     assert(this->shape.size() == 3); // TODO: || this->shape.size() == 2

//     bool no_padding = true;
//     for (size_t i = 0; i < padding.size(); i++)
//     {
//         assert(padding[i] <= this->padding[i]);

//         if (padding[i] > 0)
//         {
//             no_padding = false;
//             break;
//         }
//     }
//     if (no_padding)
//         return *this; // return directly if no padding at all

//     if (this->shape.size() == 3)
//     {
//         // top
//         int w_padding = (this->shape[1] + padding[2] + padding[3]) * this->shape[2];
//         T *start_ptr = this->get_element_ptr(padding);
//         int w_offset_0 = this->shape[1] * this->shape[2];

//         for (int i = 0; i < padding[0]; ++i)
//         {
//             dl::tool::set_value(start_ptr, value, w_padding);
//             start_ptr += w_offset_0;
//         }

//         // left & right
//         int w_offset_1 = (padding[2] + this->shape[1]) * this->shape[2];
//         int w_offset_2 = w_offset_0 - w_offset_1;
//         int w_len_1 = padding[2] * this->shape[2];
//         int w_len_2 = padding[3] * this->shape[2];

//         for (int i = 0; i < this->shape[0]; ++i)
//         {
//             dl::tool::set_value(start_ptr, value, w_len_1);
//             start_ptr += w_offset_1;
//             dl::tool::set_value(start_ptr, value, w_len_2);
//             start_ptr += w_offset_2;
//         }

//         // bottom
//         for (int i = 0; i < padding[1]; ++i)
//         {
//             dl::tool::set_value(start_ptr, value, w_padding);
//             start_ptr += w_offset_0;
//         }
//     }

//     else if (this->shape.size() == 2)
//     {
//         printf("Tensor.set_padding_value with this->shape.size() == 2 not implement yet.\n");
//     }

//     return *this;
// }
// template Tensor<uint16_t> &Tensor<uint16_t>::set_padding_value(const vector<int> padding, uint16_t value);
// template Tensor<uint8_t> &Tensor<uint8_t>::set_padding_value(const vector<int> padding, uint8_t value);
// template Tensor<int32_t> &Tensor<int32_t>::set_padding_value(const vector<int> padding, int32_t value);
// template Tensor<int16_t> &Tensor<int16_t>::set_padding_value(const vector<int> padding, int16_t value);
// template Tensor<int8_t> &Tensor<int8_t>::set_padding_value(const vector<int> padding, int8_t value);
// template Tensor<float> &Tensor<float>::set_padding_value(const vector<int> padding, float value);

} // namespace dl
