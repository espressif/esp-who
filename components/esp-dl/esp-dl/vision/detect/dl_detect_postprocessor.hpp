#pragma once
#include "dl_detect_define.hpp"
#include "dl_model_base.hpp"
#include "dl_tensor_base.hpp"
#include <list>
#include <map>

namespace dl {
namespace detect {
class DetectPostprocessor {
protected:
    Model *m_model;
    const float m_score_thr; /*<! Candidate box with lower score than score_thr will be filtered */
    const float m_nms_thr;   /*<! Candidate box with higher IoU than nms_thr will be filtered */
    const int m_top_k;       /*<! Keep top_k number of candidate boxes */
    float m_resize_scale_x;
    float m_resize_scale_y;
    float m_top_left_x;
    float m_top_left_y;
    std::list<result_t> m_box_list; /*<! Detected box list */

public:
    DetectPostprocessor(Model *model, const float score_thr, const float nms_thr, const int top_k) :
        m_model(model), m_score_thr(score_thr), m_nms_thr(nms_thr), m_top_k(top_k) {};
    virtual ~DetectPostprocessor() {};
    virtual void postprocess() = 0;
    void nms();
    void set_resize_scale_x(float resize_scale_x) { m_resize_scale_x = resize_scale_x; };
    void set_resize_scale_y(float resize_scale_y) { m_resize_scale_y = resize_scale_y; };
    void set_top_left_x(float top_left_x) { m_top_left_x = top_left_x; };
    void set_top_left_y(float top_left_y) { m_top_left_y = top_left_y; };
    void clear_result() { m_box_list.clear(); };
    std::list<result_t> &get_result(int width, int height);
};

class AnchorPointDetectPostprocessor : public DetectPostprocessor {
protected:
    std::vector<anchor_point_stage_t> m_stages;

public:
    AnchorPointDetectPostprocessor(Model *model,
                                   const float score_thr,
                                   const float nms_thr,
                                   const int top_k,
                                   const std::vector<anchor_point_stage_t> &stages) :
        DetectPostprocessor(model, score_thr, nms_thr, top_k), m_stages(stages) {};
};

class AnchorBoxDetectPostprocessor : public DetectPostprocessor {
protected:
    std::vector<anchor_box_stage_t> m_stages;

public:
    AnchorBoxDetectPostprocessor(Model *model,
                                 const float score_thr,
                                 const float nms_thr,
                                 const int top_k,
                                 const std::vector<anchor_box_stage_t> &stages) :
        DetectPostprocessor(model, score_thr, nms_thr, top_k), m_stages(stages) {};
};
} // namespace detect
} // namespace dl
