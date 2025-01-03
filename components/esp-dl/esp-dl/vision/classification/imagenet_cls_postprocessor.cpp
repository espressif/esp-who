#include "imagenet_cls_postprocessor.hpp"
#include "imagenet_category_name.hpp"

namespace dl {
namespace cls {
ImageNetClsPostprocessor::ImageNetClsPostprocessor(
    Model *model, const int top_k, const float score_thr, bool need_softmax, const std::string &output_name) :
    ClsPostprocessor(model, top_k, score_thr, need_softmax, output_name)
{
    m_cat_names = imagenet_cat_names;
}
} // namespace cls
} // namespace dl
