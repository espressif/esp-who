#include "dl_base_max2d.hpp"

#include "dl_base_activate_output.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t>
inline void max2d_11c(feature_t *output_ptr,
                      feature_t *input0_ptr,
                      feature_t *input1_ptr,
                      const arithArgsType<feature_t> &args)
{
    for (size_t output_c = 0; output_c < args.channel; output_c++) // C
    {
        output_ptr[output_c] = DL_MAX(input0_ptr[output_c], input1_ptr[output_c]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize max2d<int16_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void load_max2d_11c_s16(arith_i_impl_func_s16_t &i_impl_func,
                               arith_c_impl_func_s16_t &c_impl_func,
                               arith_n_wise_tail_s16_t &n_wise_tail,
                               const arithArgsType<int16_t> &args)
{
#if CONFIG_TIE728_BOOST
    if (args.input0_x_offset % 8 == 0 && args.input1_x_offset % 8 == 0 && args.output_x_offset % 8 == 0 &&
        !((unsigned)&args.input0_element[0] & 15) && !((unsigned)&args.input1_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s16_max2d_11c;
    } else {
        i_impl_func = dl_tie728_s16_unaligned_max2d_11c;
    }

#else
    c_impl_func = max2d_11c<int16_t>;
#endif // CONFIG_TIE728_BOOST
}

template <>
void max2d<int16_t>(void *const args_ptr)
{
    const arithArgsType<int16_t> &args = *((arithArgsType<int16_t> *)args_ptr);

    arith_i_impl_func_s16_t i_impl_func = NULL;
    arith_c_impl_func_s16_t c_impl_func = NULL;
    arith_n_wise_tail_s16_t n_wise_tail = NULL;

    load_max2d_11c_s16(i_impl_func, c_impl_func, n_wise_tail, args);

    arith_operation_shell<int16_t>(args, i_impl_func, c_impl_func, n_wise_tail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize max2d<int8_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void load_max2d_11c_s8(arith_i_impl_func_s8_t &i_impl_func,
                              arith_c_impl_func_s8_t &c_impl_func,
                              arith_n_wise_tail_s8_t &n_wise_tail,
                              const arithArgsType<int8_t> &args)
{
#if CONFIG_TIE728_BOOST
    if (args.input0_x_offset % 16 == 0 && args.input1_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 &&
        !((unsigned)&args.input0_element[0] & 15) && !((unsigned)&args.input1_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s8_max2d_11c;
    } else {
        i_impl_func = dl_tie728_s8_unaligned_max2d_11c;
    }
#else
    c_impl_func = max2d_11c<int8_t>;
#endif // CONFIG_TIE728_BOOST
}

template <>
void max2d<int8_t>(void *const args_ptr)
{
    const arithArgsType<int8_t> &args = *((arithArgsType<int8_t> *)args_ptr);

    arith_i_impl_func_s8_t i_impl_func = NULL;
    arith_c_impl_func_s8_t c_impl_func = NULL;
    arith_n_wise_tail_s8_t n_wise_tail = NULL;

    load_max2d_11c_s8(i_impl_func, c_impl_func, n_wise_tail, args);

    arith_operation_shell<int8_t>(args, i_impl_func, c_impl_func, n_wise_tail);
}
} // namespace base
} // namespace dl
