#pragma once
#include "dl_base_div.hpp"
#include "dl_base_shape.hpp"
#include "dl_module_base.hpp"

namespace dl {
namespace module {
/**
 * @brief: Performs element-wise binary subtraction (with Numpy-style broadcasting support).
 *         Please refer to https://onnx.ai/onnx/operators/onnx__Div.html for more details
 *
 */

class Div : public Module {
private:
    void *m_args;
    std::vector<TensorBase *>
        m_inputs_constant; /*input of add. If the input of add is constant , store its TensorBase pointer; if not, pass
                          in nullptr. This container can be empty, but if values are provided, they must adhere to the
                          order and quantity defined in ONNX.*/

public:
    /**
     * @brief Construct a new Div object.
     *
     * @param name              name of module
     * @param inplace           inplace type.
     * @param quant_type        quantize type.
     * @param inputs_constant   if there are constant inputs, pass them through this parameter.
     */
    Div(const char *name = NULL,
        module_inplace_t inplace = MODULE_NON_INPLACE,
        quant_type_t quant_type = QUANT_TYPE_NONE,
        std::vector<TensorBase *> inputs_constant = {}) :
        Module(name, inplace, quant_type), m_inputs_constant(inputs_constant)
    {
        m_args = nullptr;
    }

    /**
     * @brief Destroy the Div object.
     */
    ~Div()
    {
        if (m_args) {
            if (quant_type == QUANT_TYPE_SYMM_8BIT) {
                base::elemwiseArgsType<int8_t> *args = (base::elemwiseArgsType<int8_t> *)m_args;
                if (args->table) {
                    free(args->table);
                }
                free(args);
            } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
                base::elemwiseArgsType<int16_t> *args = (base::elemwiseArgsType<int16_t> *)m_args;
                if (args->table) {
                    free(args->table);
                }
                free(args);
            }
        }

        for (int i = 0; i < m_inputs_constant.size(); i++) {
            if (m_inputs_constant[i]) {
                delete m_inputs_constant[i];
            }
        }
        m_inputs_constant.clear();
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        std::vector<std::vector<int>> retrieve_input_shapes = retrieve_inputs_shape(input_shapes, m_inputs_constant);

        std::vector<int> output_shape =
            base::get_multidirectional_broadcasting_shape(retrieve_input_shapes[0], retrieve_input_shapes[1]);

        return std::vector<std::vector<int>>(1, output_shape);
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            forward_template<int8_t>(tensors, mode);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            forward_template<int16_t>(tensors, mode);
        }
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::elemwise_div((base::elemwiseArgsType<int8_t> *)args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::elemwise_div((base::elemwiseArgsType<int16_t> *)args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        std::vector<TensorBase *> inputs = retrieve_inputs(tensors, m_inputs_constant);
        TensorBase *output = tensors[m_outputs_index[0]];

        if (m_args) {
            forward_args(m_args);
        } else {
            m_args = (void *)base::get_elemwise_div_args<T>(
                output, inputs[0], inputs[1], mode); // get element-wise operation args
            forward_args(m_args);
        }
    }

    /**
     * @brief deserialize Div module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        //
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            TensorBase *input0_constant = fbs_model->get_operation_parameter(node_name, 0);
            TensorBase *input1_constant = fbs_model->get_operation_parameter(node_name, 1);
            op = new Div(node_name.c_str(), MODULE_NON_INPLACE, quant_type, {input0_constant, input1_constant});
        }
        return op;
    }

    void print()
    {
        ESP_LOGI("Div",
                 "quant_type: %s, input feature map size: %d.",
                 quant_type_to_string(quant_type),
                 m_inputs_index.size());
    }
};

} // namespace module
} // namespace dl
