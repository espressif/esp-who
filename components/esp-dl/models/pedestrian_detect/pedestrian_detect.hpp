#pragma once

#include "dl_detect_base.hpp"
#include "dl_detect_pico_postprocessor.hpp"

namespace pedestrian_detect {
class Pico : public dl::detect::DetectImpl {
public:
    Pico(const char *model_name);
};
} // namespace pedestrian_detect

class PedestrianDetect : public dl::detect::DetectWrapper {
public:
    typedef enum { PICO_S8_V1 } model_type_t;
    PedestrianDetect(const char *sdcard_model_dir = nullptr,
                     model_type_t model_type = static_cast<model_type_t>(CONFIG_PEDESTRIAN_DETECT_MODEL_TYPE));
};
