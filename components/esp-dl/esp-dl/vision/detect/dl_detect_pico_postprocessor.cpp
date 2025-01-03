#include "dl_detect_pico_postprocessor.hpp"
#include "dl_math.hpp"
#include <algorithm>
#include <cmath>

namespace dl {
namespace detect {
template <typename T>
void PicoPostprocessor::parse_stage(TensorBase *score, TensorBase *box, const int stage_index)
{
    int stride_y = m_stages[stage_index].stride_y;
    int stride_x = m_stages[stage_index].stride_x;

    int offset_y = m_stages[stage_index].offset_y;
    int offset_x = m_stages[stage_index].offset_x;

    int H = score->shape[1];
    int W = score->shape[2];
    int C = score->shape[3];

    T *score_ptr = (T *)score->data;
    T *box_ptr = (T *)box->data;
    float score_exp = DL_SCALE(score->exponent);
    float box_exp = DL_SCALE(box->exponent);
    T score_thr_quant = quantize<T>(m_score_thr * m_score_thr, 1.f / score_exp);
    float inv_resize_scale_x = 1.f / m_resize_scale_x;
    float inv_resize_scale_y = 1.f / m_resize_scale_y;

    for (size_t y = 0; y < H; y++) // height
    {
        for (size_t x = 0; x < W; x++) // width
        {
            for (size_t c = 0; c < C; c++) // category number
            {
                if (*score_ptr > score_thr_quant) {
                    int center_y = y * stride_y + offset_y;
                    int center_x = x * stride_x + offset_x;

                    float box_data[32];
                    for (int i = 0; i < 32; i++) {
                        box_data[i] = dequantize(box_ptr[i], box_exp);
                    }

                    result_t new_box = {
                        (int)c,
                        sqrtf(dequantize(*score_ptr, score_exp)),
                        {(int)((center_x - dl::math::dfl_integral(box_data, 7) * stride_x) * inv_resize_scale_x),
                         (int)((center_y - dl::math::dfl_integral(box_data + 8, 7) * stride_y) * inv_resize_scale_y),
                         (int)((center_x + dl::math::dfl_integral(box_data + 16, 7) * stride_x) * inv_resize_scale_x),
                         (int)((center_y + dl::math::dfl_integral(box_data + 24, 7) * stride_y) * inv_resize_scale_y)},
                        {}};

                    m_box_list.insert(std::upper_bound(m_box_list.begin(), m_box_list.end(), new_box, greater_box),
                                      new_box);
                }
            }
            score_ptr++;
            box_ptr += 32;
        }
    }
}

template void PicoPostprocessor::parse_stage<int8_t>(TensorBase *score, TensorBase *box, const int stage_index);
template void PicoPostprocessor::parse_stage<int16_t>(TensorBase *score, TensorBase *box, const int stage_index);

void PicoPostprocessor::postprocess()
{
    TensorBase *score0 = m_model->get_intermediate("score0");
    TensorBase *bbox0 = m_model->get_intermediate("bbox0");
    TensorBase *score1 = m_model->get_intermediate("score1");
    TensorBase *bbox1 = m_model->get_intermediate("bbox1");
    TensorBase *score2 = m_model->get_intermediate("score2");
    TensorBase *bbox2 = m_model->get_intermediate("bbox2");

    if (score0->dtype == DATA_TYPE_INT8) {
        parse_stage<int8_t>(score0, bbox0, 0);
        parse_stage<int8_t>(score1, bbox1, 1);
        parse_stage<int8_t>(score2, bbox2, 2);
    } else {
        parse_stage<int16_t>(score0, bbox0, 0);
        parse_stage<int16_t>(score1, bbox1, 1);
        parse_stage<int16_t>(score2, bbox2, 2);
    }
    nms();
}
} // namespace detect
} // namespace dl
