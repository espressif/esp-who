#pragma once

#include "dl_base_mul2d.hpp"
#include "dl_module_base.hpp"

namespace dl {
namespace module {
/**
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class Transpose : public Module {
private:
    std::vector<int> m_perm; /*<! A list of integers. Its length must be equal to the rank of the input. >*/

public:
    /**
     * @brief Construct a new Transpose object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Transpose(const char *name = NULL,
              module_inplace_t inplace = MODULE_NON_INPLACE,
              quant_type_t quant_type = QUANT_TYPE_NONE,
              std::vector<int> perm = {}) :
        Module(name, inplace, quant_type), m_perm(perm)
    {
    }

    /**
     * @brief Destroy the Transpose object.
     */
    ~Transpose() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        assert(input_shapes[0].size() == m_perm.size() || m_perm.size() == 0);

        std::vector<int> output_shape;

        for (int i = 0; i < input_shapes[0].size(); i++) {
            if (m_perm[i] < 0) {
                m_perm[i] = m_perm.size() + m_perm[i];
            }
            output_shape.push_back(input_shapes[0][m_perm[i]]);
        }

        std::vector<std::vector<int>> output_shapes(1, output_shape);
        return output_shapes;
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        output->transpose(input, m_perm);
        DL_LOG_LAYER_LATENCY_END(this->name, "Transpose");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Transpose module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        std::vector<int> perm;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "perm", perm);

        // Create module
        op = new Transpose(node_name.c_str(), MODULE_NON_INPLACE, quant_type, perm);
        return op;
    }

    void print()
    {
        ESP_LOGI(
            "Transpose", "quant_type: %s. perm: %s", quant_type_to_string(quant_type), shape_to_string(m_perm).c_str());
    }
};
} // namespace module
} // namespace dl
