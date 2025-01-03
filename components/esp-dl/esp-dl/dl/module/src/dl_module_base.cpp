#include "dl_module_base.hpp"
#include <string.h>

static const char *TAG = "dl::module::Module";

using namespace dl;

namespace dl {
namespace module {
Module::Module(const char *name, module_inplace_t inplace, quant_type_t quant_type) :
    inplace(inplace), quant_type(quant_type)
{
#if CONFIG_DL_DEBUG
    if (name) {
        int length = strlen(name) + 1;
        this->name = (char *)malloc(sizeof(char) * length);
        memcpy(this->name, name, length);
    } else {
        this->name = NULL;
    }
#else
    this->name = NULL;
#endif
}

Module::~Module()
{
    if (this->name) {
        free((void *)this->name);
    }
}

std::vector<std::vector<int>> Module::retrieve_inputs_shape(std::vector<std::vector<int>> &input_shapes,
                                                            std::vector<dl::TensorBase *> inputs)
{
    if (inputs.size() > 0 && inputs.size() < m_inputs_index.size()) {
        ESP_LOGE(TAG, "The order and quantity of the parameters passed must be consistent with those defined in ONNX.");
        assert(inputs.size() >= m_inputs_index.size());
    }

    if (inputs.size() == 0) {
        return input_shapes;
    } else {
        std::vector<std::vector<int>> output_shapes(inputs.size());
        for (int i = 0, j = 0; i < inputs.size(); i++) {
            if (inputs[i]) {
                output_shapes[i] = inputs[i]->get_shape();
            } else {
                output_shapes[i] = input_shapes[j];
                j++;
            }
        }
        return output_shapes;
    }
}

std::vector<TensorBase *> Module::retrieve_inputs(std::vector<TensorBase *> &tensors,
                                                  std::vector<dl::TensorBase *> inputs)
{
    if (inputs.size() > 0 && inputs.size() < m_inputs_index.size()) {
        ESP_LOGE(TAG, "The order and quantity of the parameters passed must be consistent with those defined in ONNX.");
        assert(inputs.size() >= m_inputs_index.size());
    }

    if (inputs.size() == 0) {
        std::vector<TensorBase *> ret_inputs(m_inputs_index.size());
        for (int i = 0; i < m_inputs_index.size(); i++) {
            ret_inputs[i] = tensors[m_inputs_index[i]];
        }
        return ret_inputs;
    } else {
        std::vector<TensorBase *> ret_inputs(inputs.size());
        for (int i = 0, j = 0; i < inputs.size(); i++) {
            if (inputs[i]) {
                ret_inputs[i] = inputs[i];
            } else {
                ret_inputs[i] = tensors[m_inputs_index[j]];
                j++;
            }
        }
        return ret_inputs;
    }
}

void Module::run(TensorBase *input, TensorBase *output, runtime_mode_t mode)
{
    std::vector<dl::TensorBase *> tensors = {input, output};
    m_inputs_index.push_back(0);
    m_outputs_index.push_back(1);
    forward(tensors, mode);
}

void Module::run(std::vector<dl::TensorBase *> inputs, std::vector<dl::TensorBase *> outputs, runtime_mode_t mode)
{
    std::vector<dl::TensorBase *> tensors;
    for (int i = 0; i < inputs.size(); i++) {
        tensors.push_back(inputs[i]);
        m_inputs_index.push_back(tensors.size() - 1);
    }

    for (int i = 0; i < outputs.size(); i++) {
        tensors.push_back(outputs[i]);
        m_outputs_index.push_back(tensors.size() - 1);
    }

    forward(tensors, mode);
}

} // namespace module
} // namespace dl
