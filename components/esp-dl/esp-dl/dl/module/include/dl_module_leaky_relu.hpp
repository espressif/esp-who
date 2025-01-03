#pragma once

#include "dl_module_base.hpp"
#include "dl_module_lut.hpp"

namespace dl {
namespace module {
/**
 * NOTE:
 *
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t, implemented by LUT
 *         - int8_t: stands for operation in int16_t, implemented by LUT
 *         refer to https://onnx.ai/onnx/operators/onnx__LeakyRelu.html
 */
class LeakyRelu : public Module {
private:
    float alpha;

public:
    /**
     * @brief Construct a new LeakyRelu object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    LeakyRelu(const char *name = NULL,
              float alpha = 0.01,
              module_inplace_t inplace = MODULE_NON_INPLACE,
              quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type)
    {
        this->alpha = alpha;
    }

    /**
     * @brief Destroy the LeakyRelu object.
     */
    ~LeakyRelu() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        std::vector<std::vector<int>> output_shapes(1, input_shapes[0]);
        return output_shapes;
    }

    void forward(std::vector<TensorBase *> &tensors, runtime_mode_t mode = RUNTIME_MODE_AUTO)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            forward_template<int8_t>(tensors, mode);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            forward_template<int16_t>(tensors, mode);
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "LeakyRelu");
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        T *input_ptr = (T *)input->get_element_ptr();
        T *output_ptr = (T *)output->get_element_ptr();

        float input_scale = DL_SCALE(input->exponent);
        float output_scale = DL_RESCALE(output->exponent);
        for (size_t i = 0; i < input->size; i++) {
            float temp = input_ptr[i] * input_scale;
            if (temp >= 0) {
                tool::truncate(output_ptr[i], tool::round(temp * output_scale));
            } else {
                tool::truncate(output_ptr[i], tool::round(temp * output_scale * this->alpha));
            }
        }
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize LeakyRelu module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        float alpha = 0.01;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "alpha", alpha);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            TensorBase *table = fbs_model->get_operation_lut(node_name);
            if (table) {
                op = new LUT(node_name.c_str(), table, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
            } else {
                op = new LeakyRelu(node_name.c_str(), alpha, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
            }
        } else {
            op = new LeakyRelu(node_name.c_str(), alpha, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }

        return op;
    }

    void print() { ESP_LOGI("LeakyRelu", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
