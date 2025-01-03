#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Reshape.html
class Reshape : public Module {
private:
    TensorBase *m_shape; /*<! Specified shape for output >*/

public:
    /**
     * @brief Construct a new Reshape object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Reshape(TensorBase *shape,
            const char *name = NULL,
            module_inplace_t inplace = MODULE_NON_INPLACE,
            quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_shape(shape)
    {
    }

    /**
     * @brief Destroy the Reshape object.
     */
    ~Reshape()
    {
        if (m_shape) {
            delete m_shape;
            m_shape = nullptr;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);

        int input_size = 1;
        for (int i = 0; i < input_shapes[0].size(); i++) {
            assert(input_shapes[0][i] > 0);
            input_size *= input_shapes[0][i];
        }

        int64_t *shape_param = static_cast<int64_t *>(m_shape->get_element_ptr());
        int negative_index = -1;
        int shape_param_size = 1;
        for (int i = 0; i < m_shape->get_size(); i++) {
            if (negative_index == -1 && shape_param[i] == -1) {
                negative_index = i;
            } else if (shape_param[i] > 0) {
                shape_param_size *= shape_param[i];
            } else {
                assert(false);
            }
        }

        std::vector<int> output(m_shape->get_size());
        if (negative_index == -1) {
            assert(shape_param_size == input_size);
            for (int i = 0; i < m_shape->get_size(); i++) {
                output[i] = static_cast<int>(shape_param[i]);
            }
        } else {
            assert(input_size % shape_param_size == 0);
            for (int i = 0; i < m_shape->get_size(); i++) {
                if (i == negative_index) {
                    output[i] = input_size / shape_param_size;
                } else {
                    output[i] = static_cast<int>(shape_param[i]);
                }
            }
        }
        std::vector<std::vector<int>> output_shapes(1, output);
        return output_shapes;
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        assert(input->get_size() == output->get_size());
        if (output->get_element_ptr() != input->get_element_ptr()) {
            output->assign(input);
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "Reshape");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Reshape module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *shape = fbs_model->get_operation_parameter(node_name, 1);

        // Create module
        op = new Reshape(shape, node_name.c_str(), MODULE_INPLACE_UNCHANGED_BUFFER, quant_type);
        return op;
    }

    void print()
    {
        ESP_LOGI("Reshape",
                 "quant_type: %s, shape: %s.",
                 quant_type_to_string(quant_type),
                 shape_to_string(m_shape->get_shape()).c_str());
    }
};
} // namespace module
} // namespace dl
