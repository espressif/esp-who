#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "dl_constant.hpp"

using namespace std;

namespace dl {
template <typename T>
Constant<T>::Constant(const T *element,
                      const int exponent,
                      const vector<int> shape,
                      const bool dynamic_alloc,
                      const dl::memory_relayout_type_t memory_relayout) :
    element(element), exponent(exponent), shape(shape), dynamic_alloc(dynamic_alloc)
{
    if (this->dynamic_alloc) {
        int size = 1;
        int alloc_size = 1;
        for (int i = 0; i < shape.size(); ++i) {
            assert(shape[i] >= 0);
            size *= shape[i];
        }
        // Apply for more memory to avoid reading data from illegal addresses when loading data into vector registers.
        if ((sizeof(T) == 1 || sizeof(T) == 4) && (size & 15)) {
            alloc_size = ((size >> 4) + 1) << 4;
        } else if (sizeof(T) == 2 && (size & 7)) {
            alloc_size = ((size >> 3) + 1) << 3;
        } else {
            alloc_size = size;
        }

        if (memory_relayout == dl::MEMORY_RELAYOUT_INTERTWINE_32_32 && sizeof(T) == 4) {
            this->element = (T *)tool::calloc_aligned(alloc_size, sizeof(T) * 2, 16, MALLOC_CAP_8BIT);

            const int32_t *input_element_tmp = reinterpret_cast<const int32_t *>(element);
            int64_t *element_tmp = reinterpret_cast<int64_t *>(const_cast<T *>(this->element));
            for (int i = 0; i < size; i++) {
                element_tmp[i] = input_element_tmp[i];
            }
        } else {
            this->element = (T *)tool::calloc_aligned(alloc_size, sizeof(T), 16, MALLOC_CAP_8BIT);
            tool::copy_memory(const_cast<T *>(this->element), const_cast<T *>(element), size * sizeof(T));
        }
    }
}
template Constant<int16_t>::Constant(const int16_t *element,
                                     const int exponent,
                                     const vector<int> shape,
                                     const bool dynamic_alloc,
                                     const dl::memory_relayout_type_t memory_relayout);
template Constant<int8_t>::Constant(const int8_t *element,
                                    const int exponent,
                                    const vector<int> shape,
                                    const bool dynamic_alloc,
                                    const dl::memory_relayout_type_t memory_relayout);
template Constant<int32_t>::Constant(const int32_t *element,
                                     const int exponent,
                                     const vector<int> shape,
                                     const bool dynamic_alloc,
                                     const dl::memory_relayout_type_t memory_relayout);

template <typename T>
Constant<T>::~Constant()
{
    if (this->dynamic_alloc) {
        if (this->element) {
            tool::free_aligned(const_cast<T *>(this->element));
        }
    }
}
template Constant<int16_t>::~Constant();
template Constant<int8_t>::~Constant();
template Constant<int32_t>::~Constant();

template <typename T>
Filter<T>::Filter(const T *element,
                  const int exponent,
                  const std::vector<int> shape,
                  const std::vector<int> dilation,
                  const bool dynamic_alloc) :
    Constant<T>(element, exponent, shape, dynamic_alloc), dilation(dilation), channel_exponent_size(1)
{
    this->shape_with_dilation = shape;
    for (int i = 0; i < dilation.size(); i++) this->shape_with_dilation[i] = (shape[i] - 1) * dilation[i] + 1;
}
template Filter<int16_t>::Filter(const int16_t *element,
                                 const int exponent,
                                 const std::vector<int> shape,
                                 const std::vector<int> dilation,
                                 const bool dynamic_alloc);
template Filter<int8_t>::Filter(const int8_t *element,
                                const int exponent,
                                const std::vector<int> shape,
                                const std::vector<int> dilation,
                                const bool dynamic_alloc);

template <typename T>
Filter<T>::Filter(const T *element,
                  const int8_t *channel_exponent,
                  const int channel_exponent_size,
                  const std::vector<int> shape,
                  const std::vector<int> dilation) :
    Constant<T>(element, INT_MIN, shape),
    dilation(dilation),
    channel_exponent(channel_exponent),
    channel_exponent_size(channel_exponent_size)
{
    this->shape_with_dilation = shape;
    for (int i = 0; i < dilation.size(); i++) this->shape_with_dilation[i] = (shape[i] - 1) * dilation[i] + 1;
}
template Filter<int8_t>::Filter(const int8_t *element,
                                const int8_t *channel_exponent,
                                const int channel_exponent_size,
                                const std::vector<int> shape,
                                const std::vector<int> dilation);

template <typename T>
void Filter<T>::print2d_n(const int n, const char *message) const
{
    printf("%s\n", message);

    for (int y = 0; y < this->shape[0]; y++) {
        for (int x = 0; x < this->shape[1]; x++) {
            printf("(");
            for (size_t c = 0; c < this->shape[2]; c++) {
                printf("%7d", this->element[((y * this->shape[1] + x) * this->shape[2] + c) * this->shape[3] + n]);
            }
            printf(")");
        }
        printf("\n");
    }
}
template void Filter<int16_t>::print2d_n(const int n, const char *message) const;
template void Filter<int8_t>::print2d_n(const int n, const char *message) const;

template <typename T>
Activation<T>::Activation(const activation_type_t type,
                          const T *element,
                          const int exponent,
                          const vector<int> shape,
                          const bool dynamic_alloc) :
    Constant<T>(element, exponent, shape, dynamic_alloc), type(type)
{
}
template Activation<int16_t>::Activation(const activation_type_t type,
                                         const int16_t *element,
                                         const int exponent,
                                         const vector<int> shape,
                                         const bool dynamic_alloc);
template Activation<int8_t>::Activation(const activation_type_t type,
                                        const int8_t *element,
                                        const int exponent,
                                        const vector<int> shape,
                                        const bool dynamic_alloc);
} // namespace dl
