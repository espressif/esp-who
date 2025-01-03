#include "dl_base.hpp"
#include "dl_base_elemwise.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {

template <typename feature_t>
feature_t *create_div_lut(elemwiseArgsType<feature_t> *args)
{
    feature_t *table = nullptr;
    feature_t *input0 = args->input0_element;
    feature_t *input1 = args->input1_element;
    float rescale = args->output_rescale;

    if (sizeof(feature_t) == 1) {
        if (args->input0_d0 == 1) {
            table = (feature_t *)tool::malloc_aligned(256, 1, 16, MALLOC_CAP_8BIT);
            float input0_float = input0[0] * rescale;
            for (int32_t i = -128; i <= 127; i++) {
                if (i == 0) {
                    if (input0_float > 0) {
                        table[i + 128] = 127; // inf
                    } else {
                        table[i + 128] = -128; // -inf
                    }
                } else {
                    float temp = input0_float / i;
                    tool::truncate<int32_t>(table[i + 128], tool::round(temp));
                }
            }
        } else if (args->input1_d0 == 1) {
            table = (feature_t *)tool::malloc_aligned(256, 1, 16, MALLOC_CAP_8BIT);
            float scale = rescale / input1[0];
            for (int32_t i = -128; i <= 127; i++) {
                if (i == 0) {
                    table[i + 128] = 0;
                } else {
                    float temp = i * scale;
                    tool::truncate<int32_t>(table[i + 128], tool::round(temp));
                }
            }
        }
    }

    return table;
}

template <typename feature_t>
elemwiseArgsType<feature_t> *get_elemwise_div_args(TensorBase *output,
                                                   TensorBase *input0,
                                                   TensorBase *input1,
                                                   const runtime_mode_t runtime_mode)
{
    std::vector<elemwiseArgsType<feature_t>> m_args =
        get_elemwise_operation_args<feature_t>(output, input0, input1, runtime_mode);

    assert(m_args.size() == 1);
    elemwiseArgsType<feature_t> *args = (elemwiseArgsType<feature_t> *)tool::malloc_aligned(
        1, sizeof(elemwiseArgsType<feature_t>), 16, MALLOC_CAP_8BIT);
    memcpy(args, m_args.data(), sizeof(elemwiseArgsType<feature_t>));

    args->output_rescale = args->output_rescale * args->input0_scale / args->input1_scale;
    if ((input1->get_size() == 1 || input0->get_size() == 1) && sizeof(feature_t) == 1) {
        args->table = create_div_lut(args);
    } else {
        args->table = nullptr;
    }

    return args;
}
template elemwiseArgsType<int8_t> *get_elemwise_div_args(TensorBase *output,
                                                         TensorBase *input0,
                                                         TensorBase *input1,
                                                         const runtime_mode_t runtime_mode);
template elemwiseArgsType<int16_t> *get_elemwise_div_args(TensorBase *output,
                                                          TensorBase *input0,
                                                          TensorBase *input1,
                                                          const runtime_mode_t runtime_mode);

// input0_ptr:vector, input1_ptr:scalar
template <typename feature_t>
void c_impl_div_n_1(feature_t *output_ptr,
                    feature_t *input0_ptr,
                    feature_t *input1_ptr,
                    elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;
    float rescale = args->output_rescale / input1_ptr[0];
    for (int32_t i = 0; i < length; i++) {
        if (input0_ptr[i] == 0) {
            output_ptr[i] = 0;
        } else if (input1_ptr[0] == 0) {
            if (input0_ptr[i] > 0) {
                output_ptr[i] = 127;
            } else {
                output_ptr[i] = -128;
            }
        } else {
            float out_elem = input0_ptr[i] * rescale;
            tool::truncate<int32_t>(output_ptr[i], tool::round(out_elem));
        }
    }
}

void c_impl_div_lut_n_1(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, elemwiseArgsType<int8_t> *args)
{
    int32_t length = args->output_d0;
    int8_t *table = args->table;
    for (int32_t i = 0; i < length; i++) {
        output_ptr[i] = table[input0_ptr[i] + 128];
    }
}

// input0_ptr:scalar, input1_ptr:vector
template <typename feature_t>
void c_impl_div_1_n(feature_t *output_ptr,
                    feature_t *input0_ptr,
                    feature_t *input1_ptr,
                    elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;

    float rescale = input0_ptr[0] * args->output_rescale;
    for (int32_t i = 0; i < length; i++) {
        if (input0_ptr[0] == 0) {
            output_ptr[i] = 0;
        } else if (input1_ptr[i] == 0) {
            if (input0_ptr[0] > 0) {
                output_ptr[i] = 127;
            } else {
                output_ptr[i] = -128;
            }
        } else {
            float out_elem = rescale / input1_ptr[i];
            tool::truncate<int32_t>(output_ptr[i], tool::round(out_elem));
        }
    }
}

void c_impl_div_lut_1_n(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, elemwiseArgsType<int8_t> *args)
{
    int32_t length = args->output_d0;
    int8_t *table = args->table;
    for (int32_t i = 0; i < length; i++) {
        output_ptr[i] = table[input1_ptr[i] + 128];
    }
}

// input0_ptr:vector, input1_ptr:vector
template <typename feature_t>
void c_impl_div_n_n(feature_t *output_ptr,
                    feature_t *input0_ptr,
                    feature_t *input1_ptr,
                    elemwiseArgsType<feature_t> *args)
{
    int32_t length = args->output_d0;
    float rescale = args->output_rescale;
    for (int i = 0; i < length; i++) {
        if (input0_ptr[i] == 0) {
            output_ptr[i] = 0;
        } else if (input1_ptr[i] == 0) {
            if (input0_ptr[i] > 0) {
                output_ptr[i] = 127;
            } else {
                output_ptr[i] = -128;
            }
        } else {
            float out_elem = input0_ptr[i] * rescale / input1_ptr[i];
            tool::truncate<int>(output_ptr[i], tool::round(out_elem));
        }
    }
}

void elemwise_div(elemwiseArgsType<int8_t> *args)
{
    std::function<void(int8_t *, int8_t *, int8_t *, elemwiseArgsType<int8_t> *)> elemwise_func =
        c_impl_div_n_n<int8_t>;

    if (args->input0_d0 == 1) {
        if (args->table) {
            elemwise_func = c_impl_div_lut_1_n;
        } else {
            elemwise_func = c_impl_div_1_n<int8_t>;
        }
    } else if (args->input1_d0 == 1) {
        if (args->table) {
            elemwise_func = c_impl_div_lut_n_1;
        } else {
            elemwise_func = c_impl_div_n_1<int8_t>;
        }
    }

    switch (args->dims) {
    case 1:
        elemwise_loop_1d(args, elemwise_func);
        break;
    case 2:
        elemwise_loop_2d(args, elemwise_func);
        break;
    case 3:
        elemwise_loop_3d(args, elemwise_func);
        break;
    case 4:
        elemwise_loop_4d(args, elemwise_func);
        break;
    default:
        break;
    }
}

void elemwise_div(elemwiseArgsType<int16_t> *args)
{
    std::function<void(int16_t *, int16_t *, int16_t *, elemwiseArgsType<int16_t> *)> elemwise_func =
        c_impl_div_n_n<int16_t>;

    if (args->input1_d0 == 1) {
        elemwise_func = c_impl_div_n_1<int16_t>;
    } else if (args->input0_d0 == 1) {
        elemwise_func = c_impl_div_1_n<int16_t>;
    }

    switch (args->dims) {
    case 1:
        elemwise_loop_1d(args, elemwise_func);
        break;
    case 2:
        elemwise_loop_2d(args, elemwise_func);
        break;
    case 3:
        elemwise_loop_3d(args, elemwise_func);
        break;
    case 4:
        elemwise_loop_4d(args, elemwise_func);
        break;
    default:
        break;
    }
}
// template void elemwise_div(elemwiseArgsType<int8_t> *args);
// template void elemwise_div(elemwiseArgsType<int16_t> *args);

} // namespace base
} // namespace dl
