#include "dl_detect_postprocessor.hpp"

namespace dl {
namespace detect {
void DetectPostprocessor::nms()
{
    int kept_number = 0;
    for (std::list<result_t>::iterator kept = m_box_list.begin(); kept != m_box_list.end(); kept++) {
        kept_number++;

        if (kept_number >= m_top_k) {
            m_box_list.erase(++kept, m_box_list.end());
            break;
        }

        int kept_area = (kept->box[2] - kept->box[0] + 1) * (kept->box[3] - kept->box[1] + 1);

        std::list<result_t>::iterator other = kept;
        other++;
        for (; other != m_box_list.end();) {
            int inter_lt_x = DL_MAX(kept->box[0], other->box[0]);
            int inter_lt_y = DL_MAX(kept->box[1], other->box[1]);
            int inter_rb_x = DL_MIN(kept->box[2], other->box[2]);
            int inter_rb_y = DL_MIN(kept->box[3], other->box[3]);

            int inter_height = inter_rb_y - inter_lt_y + 1;
            int inter_width = inter_rb_x - inter_lt_x + 1;

            if (inter_height > 0 && inter_width > 0) {
                int other_area = (other->box[2] - other->box[0] + 1) * (other->box[3] - other->box[1] + 1);
                int inter_area = inter_height * inter_width;
                float iou = (float)inter_area / (kept_area + other_area - inter_area);
                if (iou > m_nms_thr) {
                    other = m_box_list.erase(other);
                    continue;
                }
            }
            other++;
        }
    }
}

std::list<result_t> &DetectPostprocessor::get_result(int width, int height)
{
    for (result_t &res : m_box_list) {
        res.limit_box(width, height);
        res.limit_keypoint(width, height);
    }
    return m_box_list;
}
} // namespace detect
} // namespace dl
