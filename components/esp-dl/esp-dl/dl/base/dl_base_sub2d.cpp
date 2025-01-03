#include "dl_base_sub2d.hpp"

#include "dl_base_activate_output.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
inline void sub2d_11c(feature_t *output_ptr,
                      feature_t *input0_ptr,
                      feature_t *input1_ptr,
                      const arithArgsType<feature_t> &args)
{
    buffer_t buffer;
    for (size_t output_c = 0; output_c < args.channel; output_c++) // C
    {
        buffer = (buffer_t)input0_ptr[output_c] - (buffer_t)input1_ptr[output_c];
        tool::truncate(output_ptr[output_c], buffer);
    }
}

template <typename feature_t, typename buffer_t>
inline void sub2d_11c_rescale(feature_t *output_ptr,
                              feature_t *input0_ptr,
                              feature_t *input1_ptr,
                              const arithArgsType<feature_t> &args)
{
    buffer_t buffer;
    if (args.rescale_input < 2) {
        for (size_t output_c = 0; output_c < args.channel; output_c++) // C
        {
            buffer =
                (buffer_t)input0_ptr[output_c] - (buffer_t)(DL_RIGHT_SHIFT(input1_ptr[output_c], args.input_shift));
            buffer = DL_RIGHT_SHIFT(buffer * args.output_scale, args.output_shift);
            tool::truncate(output_ptr[output_c], buffer);
        }
    } else {
        for (size_t output_c = 0; output_c < args.channel; output_c++) // C
        {
            buffer =
                (buffer_t)(DL_RIGHT_SHIFT(input1_ptr[output_c], args.input_shift)) - (buffer_t)input0_ptr[output_c];
            buffer = DL_RIGHT_SHIFT(buffer * args.output_scale, args.output_shift);
            tool::truncate(output_ptr[output_c], buffer);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize sub2d<int16_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_sub2d_11c_s16(arith_i_impl_func_s16_t &i_impl_func,
                               arith_c_impl_func_s16_t &c_impl_func,
                               arith_n_wise_tail_s16_t &n_wise_tail,
                               const arithArgsType<int16_t> &args)
{
#if CONFIG_TIE728_BOOST
    if (args.input0_x_offset % 8 == 0 && args.input1_x_offset % 8 == 0 && args.output_x_offset % 8 == 0 &&
        !((unsigned)&args.input0_element[0] & 15) && !((unsigned)&args.input1_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        switch (args.activation_type) {
        case Linear:

            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s16_sub2d_11c;
            } else
                i_impl_func = dl_tie728_s16_rescale_sub2d_11c;
            break;
        case ReLU:
        case LeakyReLU:
            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s16_sub2d_11c_relu;
            } else
                i_impl_func = dl_tie728_s16_rescale_sub2d_11c_relu;
            break;
        case PReLU:
            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s16_sub2d_11c_prelu;
            } else
                i_impl_func = dl_tie728_s16_rescale_sub2d_11c_prelu;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            i_impl_func = dl_tie728_s16_unaligned_sub2d_11c;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func = dl_tie728_s16_unaligned_sub2d_11c_relu;
            break;
        case PReLU:
            i_impl_func = dl_tie728_s16_unaligned_sub2d_11c_prelu;
            break;
        }
    }
#else
    if (args.input_shift == -1)
        c_impl_func = sub2d_11c<int16_t, int32_t>;
    else
        c_impl_func = sub2d_11c_rescale<int16_t, int32_t>;

    switch (args.activation_type) {
    case Linear:
        n_wise_tail = NULL;
        break;
    case ReLU:
        n_wise_tail = arith_output_relu<int16_t, int32_t>;
        break;
    case LeakyReLU:
        n_wise_tail = arith_output_leakyrelu<int16_t, int32_t>;
        break;
    case PReLU:
        n_wise_tail = arith_output_prelu<int16_t, int32_t>;
        break;
    }
#endif // CONFIG_TIE728_BOOST
}

template <>
void sub2d<int16_t>(void *const args_ptr)
{
    const arithArgsType<int16_t> &args = *((arithArgsType<int16_t> *)args_ptr);

    arith_i_impl_func_s16_t i_impl_func = NULL;
    arith_c_impl_func_s16_t c_impl_func = NULL;
    arith_n_wise_tail_s16_t n_wise_tail = NULL;

    load_sub2d_11c_s16(i_impl_func, c_impl_func, n_wise_tail, args);

    arith_operation_shell<int16_t>(args, i_impl_func, c_impl_func, n_wise_tail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize sub2d<int8_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_sub2d_11c_s8(arith_i_impl_func_s8_t &i_impl_func,
                              arith_c_impl_func_s8_t &c_impl_func,
                              arith_n_wise_tail_s8_t &n_wise_tail,
                              const arithArgsType<int8_t> &args)
{
#if CONFIG_TIE728_BOOST
    if (args.input0_x_offset % 16 == 0 && args.input1_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 &&
        !((unsigned)&args.input0_element[0] & 15) && !((unsigned)&args.input1_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        switch (args.activation_type) {
        case Linear:

            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s8_sub2d_11c;
            } else
                i_impl_func = dl_tie728_s8_rescale_sub2d_11c;
            break;
        case ReLU:
        case LeakyReLU:
            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s8_sub2d_11c_relu;
            } else
                i_impl_func = dl_tie728_s8_rescale_sub2d_11c_relu;
            break;
        case PReLU:
            if (args.input_shift == -1) {
                i_impl_func = dl_tie728_s8_sub2d_11c_prelu;
            } else
                i_impl_func = dl_tie728_s8_rescale_sub2d_11c_prelu;
            break;
        }
    } else {
        switch (args.activation_type) {
        case Linear:
            i_impl_func = dl_tie728_s8_unaligned_sub2d_11c;
            break;
        case ReLU:
        case LeakyReLU:
            i_impl_func = dl_tie728_s8_unaligned_sub2d_11c_relu;
            break;
        case PReLU:
            i_impl_func = dl_tie728_s8_unaligned_sub2d_11c_prelu;
            break;
        }
    }
#else
    if (args.input_shift == -1)
        c_impl_func = sub2d_11c<int8_t, int16_t>;
    else
        c_impl_func = sub2d_11c_rescale<int8_t, int16_t>;

    switch (args.activation_type) {
    case Linear:
        n_wise_tail = NULL;
        break;
    case ReLU:
        n_wise_tail = arith_output_relu<int8_t, int16_t>;
        break;
    case LeakyReLU:
        n_wise_tail = arith_output_leakyrelu<int8_t, int16_t>;
        break;
    case PReLU:
        n_wise_tail = arith_output_prelu<int8_t, int16_t>;
        break;
    }
#endif // CONFIG_TIE728_BOOST
}

template <>
void sub2d<int8_t>(void *const args_ptr)
{
    const arithArgsType<int8_t> &args = *((arithArgsType<int8_t> *)args_ptr);

    arith_i_impl_func_s8_t i_impl_func = NULL;
    arith_c_impl_func_s8_t c_impl_func = NULL;
    arith_n_wise_tail_s8_t n_wise_tail = NULL;

    load_sub2d_11c_s8(i_impl_func, c_impl_func, n_wise_tail, args);

    arith_operation_shell<int8_t>(args, i_impl_func, c_impl_func, n_wise_tail);
}
} // namespace base
} // namespace dl
