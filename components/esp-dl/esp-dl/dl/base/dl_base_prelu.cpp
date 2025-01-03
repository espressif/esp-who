#include "dl_base_prelu.hpp"

#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
void prelu_11c(feature_t *output_ptr, feature_t *input_ptr, const ArgsType<feature_t> &args)
{
    buffer_t temp;
    feature_t *alpha_ptr = (feature_t *)args.activation_alpha_ptr;
    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        output_ptr[output_c] = input_ptr[output_c];
        if (output_ptr[output_c] < 0) {
            temp =
                DL_RIGHT_SHIFT((buffer_t)output_ptr[output_c] * (buffer_t)alpha_ptr[output_c], args.activation_shift);
            tool::truncate(output_ptr[output_c], temp);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize prelu<int16_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void load_prelu_11cn_s16(i_impl_acti_s16_t &i_impl_func,
                                c_impl_acti_s16_t &c_impl_func,
                                const ArgsType<int16_t> &args)
{
#if CONFIG_TIE728_BOOST
    if (args.input_stride_x_offset % 8 == 0 && args.output_x_offset % 8 == 0 &&
        !((unsigned)&args.input_element[0] & 15) && !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s16_prelu_11c;
    } else {
        i_impl_func = dl_tie728_s16_unaligned_prelu_11c;
    }
#else
    c_impl_func = prelu_11c<int16_t, int32_t>;
#endif // CONFIG_TIE_BOOST
}

template <>
void prelu<int16_t>(void *const args_ptr)
{
    const ArgsType<int16_t> &args = *((ArgsType<int16_t> *)args_ptr);

    i_impl_acti_s16_t i_impl_func = NULL;
    c_impl_acti_s16_t c_impl_func = NULL;

    load_prelu_11cn_s16(i_impl_func, c_impl_func, args);

    activation_shell<int16_t>(args, i_impl_func, c_impl_func);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// specialize prelu<int8_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void load_prelu_11cn_s8(i_impl_acti_s8_t &i_impl_func,
                               c_impl_acti_s8_t &c_impl_func,
                               const ArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.input_stride_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 &&
        !((unsigned)&args.input_element[0] & 15) && !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_esp32p4_s8_prelu_11c;
    } else {
        i_impl_func = dl_esp32p4_s8_unaligned_prelu_11c;
    }
#elif CONFIG_TIE728_BOOST
    if (args.input_stride_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 &&
        !((unsigned)&args.input_element[0] & 15) && !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s8_prelu_11c;
    } else {
        i_impl_func = dl_tie728_s8_unaligned_prelu_11c;
    }
#else
    c_impl_func = prelu_11c<int8_t, int16_t>;
#endif // CONFIG_TIE_BOOST
}

template <>
void prelu<int8_t>(void *const args_ptr)
{
    const ArgsType<int8_t> &args = *((ArgsType<int8_t> *)args_ptr);

    i_impl_acti_s8_t i_impl_func = NULL;
    c_impl_acti_s8_t c_impl_func = NULL;

#if CONFIG_ESP32P4_BOOST
    dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
#endif
    load_prelu_11cn_s8(i_impl_func, c_impl_func, args);

    activation_shell<int8_t>(args, i_impl_func, c_impl_func);
}
} // namespace base
} // namespace dl
