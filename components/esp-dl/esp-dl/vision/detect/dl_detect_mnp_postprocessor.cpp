#include "dl_detect_mnp_postprocessor.hpp"
#include "dl_math.hpp"
#include <algorithm>
#include <cmath>

namespace dl {
namespace detect {
template <typename T>
void MNPPostprocessor::parse_stage(TensorBase *score, TensorBase *box, TensorBase *landmark, const int stage_index)
{
    std::vector<std::vector<int>> &anchor_shape = m_stages[stage_index].anchor_shape;

    int H = score->shape[1];
    int W = score->shape[2];
    int A = anchor_shape.size();
    int C = score->shape[3] / A;
    T *score_ptr = (T *)score->data;
    T *box_ptr = (T *)box->data;
    T *landmark_ptr = (T *)landmark->data;
    float score_exp = DL_SCALE(score->exponent);
    float box_exp = DL_SCALE(box->exponent);
    float landmark_exp = DL_SCALE(landmark->exponent);
    float inv_resize_scale_x = 1.f / m_resize_scale_x;
    float inv_resize_scale_y = 1.f / m_resize_scale_y;

    for (size_t y = 0; y < H; y++) // height
    {
        for (size_t x = 0; x < W; x++) // width
        {
            for (size_t a = 0; a < A; a++) // anchor number
            {
                // softmax
                float scores[C];
                scores[0] = dequantize(score_ptr[0], score_exp);
                float max_score = scores[0];
                int max_score_c = 0;
                for (int i = 1; i < C; i++) {
                    scores[i] = dequantize(score_ptr[i], score_exp);
                    if (max_score < scores[i]) {
                        max_score_c = i;
                        max_score = scores[i];
                    }
                }
                float sum = 0;
                for (int i = 0; i < C; i++) {
                    sum += expf(scores[i] - max_score);
                }
                max_score = 1. / sum;

                if (max_score > m_score_thr) {
                    int anchor_h = anchor_shape[a][0];
                    int anchor_w = anchor_shape[a][1];
                    result_t new_box = {
                        max_score_c,
                        max_score,
                        {(int)(anchor_w * dequantize(box_ptr[0], box_exp) * inv_resize_scale_x + m_top_left_x),
                         (int)(anchor_h * dequantize(box_ptr[1], box_exp) * inv_resize_scale_y + m_top_left_y),
                         (int)((anchor_w * dequantize(box_ptr[2], box_exp) + anchor_w) * inv_resize_scale_x +
                               m_top_left_x),
                         (int)((anchor_h * dequantize(box_ptr[3], box_exp) + anchor_h) * inv_resize_scale_y +
                               m_top_left_y)},
                        {
                            (int)(anchor_w * dequantize(landmark_ptr[0], landmark_exp) * inv_resize_scale_x +
                                  m_top_left_x),
                            (int)(anchor_h * dequantize(landmark_ptr[1], landmark_exp) * inv_resize_scale_y +
                                  m_top_left_y),
                            (int)(anchor_w * dequantize(landmark_ptr[2], landmark_exp) * inv_resize_scale_x +
                                  m_top_left_x),
                            (int)(anchor_h * dequantize(landmark_ptr[3], landmark_exp) * inv_resize_scale_y +
                                  m_top_left_y),
                            (int)(anchor_w * dequantize(landmark_ptr[4], landmark_exp) * inv_resize_scale_x +
                                  m_top_left_x),
                            (int)(anchor_h * dequantize(landmark_ptr[5], landmark_exp) * inv_resize_scale_y +
                                  m_top_left_y),
                            (int)(anchor_w * dequantize(landmark_ptr[6], landmark_exp) * inv_resize_scale_x +
                                  m_top_left_x),
                            (int)(anchor_h * dequantize(landmark_ptr[7], landmark_exp) * inv_resize_scale_y +
                                  m_top_left_y),
                            (int)(anchor_w * dequantize(landmark_ptr[8], landmark_exp) * inv_resize_scale_x +
                                  m_top_left_x),
                            (int)(anchor_h * dequantize(landmark_ptr[9], landmark_exp) * inv_resize_scale_y +
                                  m_top_left_y),
                        }};

                    m_box_list.insert(std::upper_bound(m_box_list.begin(), m_box_list.end(), new_box, greater_box),
                                      new_box);
                }
                score_ptr += C;
                box_ptr += 4;
                landmark_ptr += 10;
            }
        }
    }
}

void MNPPostprocessor::postprocess()
{
    TensorBase *score = m_model->get_intermediate("score");
    TensorBase *bbox = m_model->get_intermediate("box");
    TensorBase *landmark = m_model->get_intermediate("landmark");
    if (score->dtype == DATA_TYPE_INT8) {
        parse_stage<int8_t>(score, bbox, landmark, 0);
    } else {
        parse_stage<int16_t>(score, bbox, landmark, 0);
    }
}
} // namespace detect
} // namespace dl
