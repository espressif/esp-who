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
 *         refer to https://onnx.ai/onnx/operators/onnx__Relu.html
 */
class Relu : public Module {
public:
    /**
     * @brief Construct a new Relu object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Relu(const char *name = NULL,
         module_inplace_t inplace = MODULE_NON_INPLACE,
         quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type)
    {
    }

    /**
     * @brief Destroy the Relu object.
     */
    ~Relu() {}

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
        } else if (quant_type == QUANT_TYPE_FLOAT32) {
            TensorBase *input = tensors[m_inputs_index[0]];
            TensorBase *output = tensors[m_outputs_index[0]];
            float *input_ptr = (float *)input->get_element_ptr();
            float *output_ptr = (float *)output->get_element_ptr();

            for (size_t i = 0; i < input->size; i++) {
                if (input_ptr[i] >= 0) {
                    output_ptr[i] = input_ptr[i];
                } else {
                    output_ptr[i] = 0;
                }
            }
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "Relu");
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        T *input_ptr = (T *)input->get_element_ptr();
        T *output_ptr = (T *)output->get_element_ptr();

        if (input->exponent == output->exponent) {
            for (size_t i = 0; i < input->size; i++) {
                if (input_ptr[i] >= 0) {
                    output_ptr[i] = input_ptr[i];
                } else {
                    output_ptr[i] = 0;
                }
            }
        } else {
            float input_scale = DL_SCALE(input->exponent);
            float output_scale = DL_RESCALE(output->exponent);
            for (size_t i = 0; i < input->size; i++) {
                float temp = input_ptr[i] * input_scale;
                if (temp >= 0) {
                    tool::truncate(output_ptr[i], tool::round(temp * output_scale));
                } else {
                    output_ptr[i] = 0;
                }
            }
        }
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Relu module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            TensorBase *table = fbs_model->get_operation_lut(node_name);
            if (table) {
                op = new LUT(node_name.c_str(), table, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
            } else {
                op = new Relu(node_name.c_str(), MODULE_INPLACE_CHANGED_BUFFER, quant_type);
            }
        } else {
            op = new Relu(node_name.c_str(), MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }

        return op;
    }

    void print() { ESP_LOGI("Relu", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
