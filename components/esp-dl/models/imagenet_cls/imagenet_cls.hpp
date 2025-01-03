#pragma once

#include "dl_cls_base.hpp"
#include "imagenet_cls_postprocessor.hpp"

namespace imagenet_cls {

class MobileNetV2 : public dl::cls::ClsImpl {
public:
    MobileNetV2(const char *model_name, const int top_k);
};

} // namespace imagenet_cls

class ImageNetCls : public dl::cls::ClsWrapper {
public:
    typedef enum { MOBILENETV2_S8_V1 } model_type_t;
    ImageNetCls(model_type_t model_type = static_cast<model_type_t>(CONFIG_IMAGENET_CLS_MODEL_TYPE),
                const int top_k = 5);
};
