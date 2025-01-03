#pragma once

#include "dl_base.hpp"

namespace dl {
namespace base {

std::vector<int> get_pad_shape(const std::vector<int> &shape, const std::vector<int> &pads);

template <typename T>
void pad1D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value);

template <typename T>
void pad2D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value);

template <typename T>
void pad3D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value);

template <typename T>
void pad4D(T *input_element,
           T *output_element,
           std::vector<int> input_shape,
           const std::vector<int> pads,
           const padding_mode_t mode,
           const T const_value);
} // namespace base
} // namespace dl
