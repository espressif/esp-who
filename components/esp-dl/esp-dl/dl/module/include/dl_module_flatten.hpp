#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Flatten.html
class Flatten : public Module {
private:
    int m_axis; /*<! Indicate up to which input dimensions (exclusive) should be flattened to the outer dimension of the
                   output. >*/

public:
    /**
     * @brief Construct a new Flatten object.
     *
     * @param name            name of module
     * @param inplace         true: the output will store to input0
     *                        false: the output will store to a separate memory
     */
    Flatten(int axis,
            const char *name = NULL,
            module_inplace_t inplace = MODULE_NON_INPLACE,
            quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_axis(axis)
    {
    }

    /**
     * @brief Destroy the Flatten object.
     */
    ~Flatten() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        std::vector<int> output_shape(2);
        int index = -1;
        if (m_axis < 0) {
            index = input_shapes[0].size() + m_axis;
        } else {
            index = m_axis;
        }
        int size = 1;
        for (int i = 0; i < index; i++) {
            size *= input_shapes[0][i];
        }
        output_shape[0] = size;

        size = 1;
        for (int i = index; i < input_shapes[0].size(); i++) {
            size *= input_shapes[0][i];
        }
        output_shape[1] = size;
        std::vector<std::vector<int>> output_shapes(1, output_shape);
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
        DL_LOG_LAYER_LATENCY_END(this->name, "Flatten");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Flatten module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        int axis = 1;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "axis", axis);

        // Create module
        op = new Flatten(axis, node_name.c_str(), MODULE_INPLACE_UNCHANGED_BUFFER, quant_type);
        return op;
    }

    void print() { ESP_LOGI("Flatten", "quant_type: %s, axis: %d.", quant_type_to_string(quant_type), m_axis); }
};
} // namespace module
} // namespace dl
