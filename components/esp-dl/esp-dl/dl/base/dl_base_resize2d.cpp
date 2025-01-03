#include "dl_base_resize2d.hpp"

#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t>
inline void resize2d_nearest_2x2_c1(feature_t *output_ptr, feature_t *input_ptr, const resizeArgsType<feature_t> &args)
{
    feature_t *output_ptr_0_0 = output_ptr;
    feature_t *output_ptr_0_1 = output_ptr + args.output_x_offset;
    feature_t *output_ptr_1_0 = output_ptr + args.output_y_offset;
    feature_t *output_ptr_1_1 = output_ptr_1_0 + args.output_x_offset;

    for (int i = 0; i < args.input_channel; i++) {
        feature_t output_value = tool::round((float)(*input_ptr++) * args.output_scale / (1 << args.output_shift));
        *(output_ptr_0_0++) = output_value;
        *(output_ptr_0_1++) = output_value;
        *(output_ptr_1_0++) = output_value;
        *(output_ptr_1_1++) = output_value;
    }
}

inline void load_resized2d_nearest_2x2_c1_s8(resize_i_impl_func_s8_t &i_impl_func,
                                             resize_c_impl_func_s8_t &c_impl_func,
                                             const resizeArgsType<int8_t> &args)
{
#if CONFIG_ESP32P4_BOOST
    if (args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_esp32p4_s8_resize2d_nearest_2x2_c1;
    } else {
        i_impl_func = dl_esp32p4_s8_unaligned_resize2d_nearest_2x2_c1;
    }
#elif CONFIG_TIE728_BOOST
    if (args.input_channel % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s8_resize2d_nearest_2x2_c1;
    } else {
        i_impl_func = dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1;
    }
#else
    c_impl_func = resize2d_nearest_2x2_c1<int8_t>;
#endif
}

template <>
void resize2d<int8_t>(void *args_ptr)
{
    const resizeArgsType<int8_t> &args = *((resizeArgsType<int8_t> *)args_ptr);
    if (args.resize_type == RESIZE_NEAREST) {
        if (args.scale_y == 2 && args.scale_x == 2) {
            resize_i_impl_func_s8_t i_impl_func = NULL;
            resize_c_impl_func_s8_t c_impl_func = NULL;
            load_resized2d_nearest_2x2_c1_s8(i_impl_func, c_impl_func, args);
            resize2d_operation_shell<int8_t>(args, i_impl_func, c_impl_func);
        }
    }
}

template <>
void resize2d<int16_t>(void *args_ptr)
{
    // const resizeArgsType<int16_t> &args = *((resizeArgsType<int16_t> *)args_ptr);
    // if (args.resize_type == RESIZE_NEAREST){
    //     if (args.scale_y == 2 && args.scale_x == 2){
    //         resize_i_impl_func_s8_t i_impl_func = NULL;
    //         resize_c_impl_func_s8_t c_impl_func = NULL;
    //         load_resized2d_nearest_2x2_c1_s16(i_impl_func, c_impl_func, args);
    //         resize2d_operation_shell<int16_t>(args, i_impl_func, c_impl_func);
    //     }
    // }
}

} // namespace base
} // namespace dl
