#include "dl_detect_msr_postprocessor.hpp"
#include "dl_math.hpp"
#include <algorithm>
#include <cmath>

namespace dl {
namespace detect {
template <typename T>
void MSRPostprocessor::parse_stage(TensorBase *score, TensorBase *box, const int stage_index)
{
    int stride_y = m_stages[stage_index].stride_y;
    int stride_x = m_stages[stage_index].stride_x;

    int offset_y = m_stages[stage_index].offset_y;
    int offset_x = m_stages[stage_index].offset_x;

    std::vector<std::vector<int>> &anchor_shape = m_stages[stage_index].anchor_shape;

    int H = score->shape[1];
    int W = score->shape[2];
    int A = anchor_shape.size();
    int C = score->shape[3] / A;
    T *score_ptr = (T *)score->data;
    T *box_ptr = (T *)box->data;
    float score_exp = DL_SCALE(score->exponent);
    float box_exp = DL_SCALE(box->exponent);
    T score_thr_quant = quantize<T>(dl::math::inverse_sigmoid(m_score_thr), 1.f / score_exp);
    float inv_resize_scale_x = 1.f / m_resize_scale_x;
    float inv_resize_scale_y = 1.f / m_resize_scale_y;

    for (size_t y = 0; y < H; y++) // height
    {
        for (size_t x = 0; x < W; x++) // width
        {
            for (size_t a = 0; a < A; a++) // anchor number
            {
                for (size_t c = 0; c < C; c++) // category number
                {
                    if (*score_ptr > score_thr_quant) {
                        int center_y = y * stride_y + offset_y;
                        int center_x = x * stride_x + offset_x;
                        int anchor_h = anchor_shape[a][0];
                        int anchor_w = anchor_shape[a][1];
                        result_t new_box = {
                            (int)c,
                            dl::math::sigmoid(dequantize(*score_ptr, score_exp)),
                            {(int)((center_x - (anchor_w >> 1) + anchor_w * dequantize(box_ptr[0], box_exp)) *
                                   inv_resize_scale_x),
                             (int)((center_y - (anchor_h >> 1) + anchor_h * dequantize(box_ptr[1], box_exp)) *
                                   inv_resize_scale_y),
                             (int)((center_x + anchor_w - (anchor_w >> 1) +
                                    anchor_w * dequantize(box_ptr[2], box_exp)) *
                                   inv_resize_scale_x),
                             (int)((center_y + anchor_h - (anchor_h >> 1) +
                                    anchor_h * dequantize(box_ptr[3], box_exp)) *
                                   inv_resize_scale_y)},
                            {}};

                        m_box_list.insert(std::upper_bound(m_box_list.begin(), m_box_list.end(), new_box, greater_box),
                                          new_box);
                    }
                    score_ptr++;
                    box_ptr += 4;
                }
            }
        }
    }
}

template void MSRPostprocessor::parse_stage<int8_t>(TensorBase *score, TensorBase *box, const int stage_index);
template void MSRPostprocessor::parse_stage<int16_t>(TensorBase *score, TensorBase *box, const int stage_index);

void MSRPostprocessor::postprocess()
{
    TensorBase *score0 = m_model->get_intermediate("score0");
    TensorBase *bbox0 = m_model->get_intermediate("box0");
    TensorBase *score1 = m_model->get_intermediate("score1");
    TensorBase *bbox1 = m_model->get_intermediate("box1");

    if (score0->dtype == DATA_TYPE_INT8) {
        parse_stage<int8_t>(score0, bbox0, 0);
        parse_stage<int8_t>(score1, bbox1, 1);
    } else {
        parse_stage<int16_t>(score0, bbox0, 0);
        parse_stage<int16_t>(score1, bbox1, 1);
    }
    nms();
}
} // namespace detect
} // namespace dl
