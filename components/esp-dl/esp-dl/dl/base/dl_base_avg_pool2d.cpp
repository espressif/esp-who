#include "dl_base_avg_pool2d.hpp"

#include "dl_base_activate_buffer.hpp"
#include "dl_base_activate_output.hpp"
#include "dl_base_isa.hpp"

namespace dl {
namespace base {
template <typename feature_t, typename buffer_t>
inline void avgpool2d_hwc1(buffer_t *buffer_ptr,
                           feature_t *input_ptr,
                           feature_t *output_ptr,
                           PoolArgsType<feature_t> &args)
{
    buffer_t input;
    buffer_t avg_pool_area_inv = 1.f / args.avg_pool_area;
    for (size_t filter_y = 0; filter_y < args.filter_height; filter_y++) // H
    {                                                                    //
        feature_t *input_yx = input_ptr;
        for (size_t filter_x = 0; filter_x < args.filter_width; filter_x++)   // W
        {                                                                     //
            for (size_t input_c = 0; input_c < args.input_channel; input_c++) // C
            {
                input = (buffer_t)input_yx[input_c] * DL_SCALE(args.input_exponent);
                buffer_ptr[input_c] += input;
            }
            input_yx += args.input_x_offset;
        }
        input_ptr += args.input_y_offset;
    }

    for (size_t output_c = 0; output_c < args.output_channel; output_c++) {
        tool::truncate(output_ptr[output_c],
                       tool::round(buffer_ptr[output_c] * avg_pool_area_inv / DL_SCALE(args.output_exponent)));
        buffer_ptr[output_c] = 0;
    }
}

inline void load_avg_pool2d_hwc1_s16(i_impl_func_s16_t &i_impl_func,
                                     i_impl_func_s16_t &i_impl_func_sp,
                                     avg_pool_c_impl_func_s16_t &c_impl_func,
                                     PoolArgsType<int16_t> &args)
{
#if CONFIG_ACCURATE_INFER
    c_impl_func = avgpool2d_hwc1<int16_t, float>;
#else
#if CONFIG_TIE728_BOOST
    if (args.input_x_offset % 8 == 0 && args.output_x_offset % 8 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s16_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_tie728_s16_avg_pool2d_22c1
                                                                             : dl_tie728_s16_avg_pool2d_hwc1;
    } else {
        i_impl_func = dl_tie728_s16_unaligned_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_tie728_s16_unaligned_avg_pool2d_22c1
                                                                             : dl_tie728_s16_unaligned_avg_pool2d_hwc1;
    }
#else
    c_impl_func = avgpool2d_hwc1<int16_t, float>;
#endif
#endif
}

template <>
void avg_pool2d<int16_t>(void *args_ptr)
{
    PoolArgsType<int16_t> &args = *((PoolArgsType<int16_t> *)args_ptr);

    i_impl_func_s16_t i_impl_func = NULL;
    i_impl_func_s16_t i_impl_func_sp = NULL;
    avg_pool_c_impl_func_s16_t c_impl_func = NULL;

    load_avg_pool2d_hwc1_s16(i_impl_func, i_impl_func_sp, c_impl_func, args);
    avg_pool_shell<int16_t, float>(args, i_impl_func, i_impl_func_sp, c_impl_func);
}

inline void load_avg_pool2d_hwc1_s8(i_impl_func_s8_t &i_impl_func,
                                    i_impl_func_s8_t &i_impl_func_sp,
                                    avg_pool_c_impl_func_s8_t &c_impl_func,
                                    PoolArgsType<int8_t> &args)
{
#if CONFIG_ACCURATE_INFER
    c_impl_func = avgpool2d_hwc1<int8_t, float>;
#else
#if CONFIG_ESP32P4_BOOST
    if (args.input_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_esp32p4_s8_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_esp32p4_s8_avg_pool2d_22c1
                                                                             : dl_esp32p4_s8_avg_pool2d_hwc1;
    } else {
        i_impl_func = dl_esp32p4_s8_unaligned_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_esp32p4_s8_unaligned_avg_pool2d_22c1
                                                                             : dl_esp32p4_s8_unaligned_avg_pool2d_hwc1;
    }
#elif CONFIG_TIE728_BOOST
    if (args.input_x_offset % 16 == 0 && args.output_x_offset % 16 == 0 && !((unsigned)&args.input_element[0] & 15) &&
        !((unsigned)&args.output_element[0] & 15)) {
        i_impl_func = dl_tie728_s8_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_tie728_s8_avg_pool2d_22c1
                                                                             : dl_tie728_s8_avg_pool2d_hwc1;
    } else {
        i_impl_func = dl_tie728_s8_unaligned_avg_pool2d_hwc1;
        i_impl_func_sp = (args.filter_height == 2 && args.filter_width == 2) ? dl_tie728_s8_unaligned_avg_pool2d_22c1
                                                                             : dl_tie728_s8_unaligned_avg_pool2d_hwc1;
    }
#else
    c_impl_func = avgpool2d_hwc1<int8_t, float>;
#endif
#endif
}

template <>
void avg_pool2d<int8_t>(void *args_ptr)
{
    PoolArgsType<int8_t> &args = *((PoolArgsType<int8_t> *)args_ptr);

    i_impl_func_s8_t i_impl_func = NULL;
    i_impl_func_s8_t i_impl_func_sp = NULL;
    avg_pool_c_impl_func_s8_t c_impl_func = NULL;
#if CONFIG_ESP32P4_BOOST
    dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
#endif

    load_avg_pool2d_hwc1_s8(i_impl_func, i_impl_func_sp, c_impl_func, args);
    avg_pool_shell<int8_t, float>(args, i_impl_func, i_impl_func_sp, c_impl_func);
}
} // namespace base
} // namespace dl
