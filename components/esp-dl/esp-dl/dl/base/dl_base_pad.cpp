#include "dl_base_pad.hpp"

namespace dl {
namespace base {

std::vector<int> get_pad_shape(const std::vector<int> &shape, const std::vector<int> &pads)
{
    std::vector<int> output_shape = std::vector<int>(shape);
    int dim = shape.size();
    assert(dim * 2 == pads.size());

    for (int i = 0; i < dim; i++) {
        output_shape[i] = shape[i] + pads[i] + pads[i + dim];
    }

    return output_shape;
}

template <typename T>
void pad1D(T *input_element,
           T *output_element,
           const std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value)
{
    int input_len = input_shape[0];
    int left = pads[0];
    int right = pads[1];
    int right_start = left + input_len;
    tool::copy_memory(output_element + left, input_element, input_len * sizeof(T));
    if (mode == PADDING_CONSTANT) {
        for (int i = 0; i < left; i++) {
            output_element[i] = const_value;
        }
        for (int i = 0; i < right; i++) {
            output_element[i + right_start] = const_value;
        }
    } else if (mode == PADDING_EDGE) {
        for (int i = 0; i < left; i++) {
            output_element[i] = input_element[0];
        }
        for (int i = 0; i < right; i++) {
            output_element[i + right_start] = input_element[input_len - 1];
        }
    } else if (mode == PADDING_REFLECT) {
        for (int i = 0; i < left; i++) {
            output_element[i] = input_element[left - i];
        }
        int start = input_len - 2;
        for (int i = 0; i < right; i++) {
            output_element[i + right_start] = input_element[start - i];
        }
    } else if (mode == PADDING_WRAP) {
        int start = input_len - 1;
        for (int i = 0; i < left; i++) {
            output_element[i] = input_element[start - i];
        }
        for (int i = 0; i < right; i++) {
            output_element[i + right_start] = input_element[i];
        }
    }
}

template void pad1D(int8_t *input_element,
                    int8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int8_t const_value);
template void pad1D(uint8_t *input_element,
                    uint8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint8_t const_value);
template void pad1D(int16_t *input_element,
                    int16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int16_t const_value);
template void pad1D(uint16_t *input_element,
                    uint16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint16_t const_value);
template void pad1D(int32_t *input_element,
                    int32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int32_t const_value);
template void pad1D(uint32_t *input_element,
                    uint32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint32_t const_value);
template void pad1D(float *input_element,
                    float *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const float const_value);

template <typename T>
void pad_head_and_tail(T *input_element,
                       T *output_element,
                       std::vector<int> input_shape,
                       const int pad_head,
                       const int pad_tail,
                       const int output_offset,
                       const padding_mode_t mode,
                       const T const_value)
{
    T *output_ptr = nullptr;
    int tail_offset = (pad_head + input_shape[0]) * output_offset;
    if (mode == PADDING_CONSTANT) {
        if (pad_head > 0) {
            tool::set_value<T>(output_element, const_value, output_offset * pad_head);
        }
        output_ptr = output_element + tail_offset;
        if (pad_tail > 0) {
            tool::set_value<T>(output_ptr, const_value, output_offset * pad_tail);
        }
    } else if (mode == PADDING_EDGE) {
        if (pad_head > 0) {
            T *edge_line = output_element + pad_head * output_offset;
            output_ptr = output_element;
            for (int i = 0; i < pad_head; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
            }
        }
        if (pad_tail > 0) {
            output_ptr = output_element + tail_offset;
            T *edge_line = output_ptr - output_offset;
            for (int i = 0; i < pad_tail; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
            }
        }
    } else if (mode == PADDING_REFLECT) {
        if (pad_head > 0) {
            T *edge_line = output_element + 2 * pad_head * output_offset;
            output_ptr = output_element;
            for (int i = 0; i < pad_head; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
                edge_line -= output_offset;
            }
        }
        if (pad_tail > 0) {
            output_ptr = output_element + tail_offset;
            T *edge_line = output_ptr - 2 * output_offset;
            for (int i = 0; i < pad_tail; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
                edge_line -= output_offset;
            }
        }
    } else if (mode == PADDING_WRAP) {
        if (pad_head > 0) {
            T *edge_line = output_element + tail_offset - output_offset;
            output_ptr = output_element;
            for (int i = 0; i < pad_head; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
                edge_line -= output_offset;
            }
        }
        if (pad_tail > 0) {
            output_ptr = output_element + tail_offset;
            T *edge_line = output_element + pad_head * output_offset;
            for (int i = 0; i < pad_tail; i++) {
                tool::copy_memory(output_ptr, edge_line, output_offset * sizeof(T));
                output_ptr += output_offset;
                edge_line += output_offset;
            }
        }
    }
}

template void pad_head_and_tail(int8_t *input_element,
                                int8_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const int8_t const_value);
template void pad_head_and_tail(uint8_t *input_element,
                                uint8_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const uint8_t const_value);
template void pad_head_and_tail(int16_t *input_element,
                                int16_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const int16_t const_value);
template void pad_head_and_tail(uint16_t *input_element,
                                uint16_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const uint16_t const_value);
template void pad_head_and_tail(int32_t *input_element,
                                int32_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const int32_t const_value);
template void pad_head_and_tail(uint32_t *input_element,
                                uint32_t *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const uint32_t const_value);
template void pad_head_and_tail(float *input_element,
                                float *output_element,
                                std::vector<int> input_shape,
                                const int pad_head,
                                const int pad_tail,
                                const int output_offset,
                                const padding_mode_t mode,
                                const float const_value);

template <typename T>
void pad2D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value)
{
    int left = pads[1];
    int pad_head = pads[0];
    int right = pads[3];
    int pad_tail = pads[2];
    int output_offset = input_shape[1] + left + right;
    int input_offset = input_shape[1];
    T *output_ptr = output_element + pad_head * output_offset;
    T *input_prt = input_element;
    for (int i = 0; i < input_shape[0]; i++) {
        pad1D(input_prt, output_ptr, {input_offset}, {left, right}, mode, const_value);
        input_prt += input_offset;
        output_ptr += output_offset;
    }

    pad_head_and_tail<T>(
        input_element, output_element, input_shape, pad_head, pad_tail, output_offset, mode, const_value);
}
template void pad2D(int8_t *input_element,
                    int8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int8_t const_value);
template void pad2D(uint8_t *input_element,
                    uint8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint8_t const_value);
template void pad2D(int16_t *input_element,
                    int16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int16_t const_value);
template void pad2D(uint16_t *input_element,
                    uint16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint16_t const_value);
template void pad2D(int32_t *input_element,
                    int32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int32_t const_value);
template void pad2D(uint32_t *input_element,
                    uint32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint32_t const_value);
template void pad2D(float *input_element,
                    float *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const float const_value);

template <typename T>
void pad3D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value)
{
    int pad_head = pads[0];
    int top = pads[1];
    int left = pads[2];
    int pad_tail = pads[3];
    int bottom = pads[4];
    int right = pads[5];
    int output_offset = (input_shape[1] + top + bottom) * (input_shape[2] + left + right);
    int input_offset = input_shape[1] * input_shape[2];
    T *output_ptr = output_element + pad_head * output_offset;
    T *input_prt = input_element;
    for (int i = 0; i < input_shape[0]; i++) {
        pad2D(input_prt, output_ptr, {input_shape[1], input_shape[2]}, {top, left, bottom, right}, mode, const_value);
        input_prt += input_offset;
        output_ptr += output_offset;
    }

    pad_head_and_tail<T>(
        input_element, output_element, input_shape, pad_head, pad_tail, output_offset, mode, const_value);
}
template void pad3D(int8_t *input_element,
                    int8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int8_t const_value);
template void pad3D(uint8_t *input_element,
                    uint8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint8_t const_value);
template void pad3D(int16_t *input_element,
                    int16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int16_t const_value);
template void pad3D(uint16_t *input_element,
                    uint16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint16_t const_value);
template void pad3D(int32_t *input_element,
                    int32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int32_t const_value);
template void pad3D(uint32_t *input_element,
                    uint32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint32_t const_value);
template void pad3D(float *input_element,
                    float *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const float const_value);

template <typename T>
void pad4D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value)
{
    int pad_head = pads[0];
    int front = pads[1];
    int top = pads[2];
    int left = pads[3];
    int pad_tail = pads[4];
    int back = pads[5];
    int bottom = pads[6];
    int right = pads[7];
    int output_offset =
        (input_shape[1] + front + back) * (input_shape[2] + top + bottom) * (input_shape[3] + left + right);
    int input_offset = input_shape[1] * input_shape[2] * input_shape[3];
    T *output_ptr = output_element + pad_head * output_offset;
    T *input_prt = input_element;
    for (int i = 0; i < input_shape[0]; i++) {
        pad3D(input_prt,
              output_ptr,
              {input_shape[1], input_shape[2], input_shape[3]},
              {front, top, left, back, bottom, right},
              mode,
              const_value);
        input_prt += input_offset;
        output_ptr += output_offset;
    }

    pad_head_and_tail<T>(
        input_element, output_element, input_shape, pad_head, pad_tail, output_offset, mode, const_value);
}
template void pad4D(int8_t *input_element,
                    int8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int8_t const_value);
template void pad4D(uint8_t *input_element,
                    uint8_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint8_t const_value);
template void pad4D(int16_t *input_element,
                    int16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int16_t const_value);
template void pad4D(uint16_t *input_element,
                    uint16_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint16_t const_value);
template void pad4D(int32_t *input_element,
                    int32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const int32_t const_value);
template void pad4D(uint32_t *input_element,
                    uint32_t *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const uint32_t const_value);
template void pad4D(float *input_element,
                    float *output_element,
                    const std::vector<int> input_shape,
                    const std::vector<int> pads,
                    const padding_mode_t mode,
                    const float const_value);

} // namespace base
} // namespace dl
