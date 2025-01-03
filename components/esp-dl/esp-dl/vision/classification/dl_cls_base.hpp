#pragma once

#include "dl_cls_postprocessor.hpp"
#include "dl_image_preprocessor.hpp"
#include "dl_model_base.hpp"

namespace dl {
namespace cls {
class Cls {
public:
    virtual ~Cls() {};
    virtual std::vector<dl::cls::result_t> &run(const dl::image::img_t &img) = 0;
};

class ClsWrapper : public Cls {
protected:
    Cls *m_model;

public:
    virtual ~ClsWrapper()
    {
        if (m_model) {
            delete m_model;
            m_model = nullptr;
        }
    }
    std::vector<dl::cls::result_t> &run(const dl::image::img_t &img) { return m_model->run(img); }
};

class ClsImpl : public Cls {
protected:
    dl::Model *m_model;
    dl::image::ImagePreprocessor *m_image_preprocessor;
    dl::cls::ClsPostprocessor *m_postprocessor;

public:
    ~ClsImpl();
    std::vector<dl::cls::result_t> &run(const dl::image::img_t &img) override;
};
} // namespace cls
} // namespace dl
