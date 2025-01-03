#pragma once

#include "dl_base_prelu.hpp"
#include "dl_module_base.hpp"
#include "dl_module_lut.hpp"

namespace dl {
namespace module {
/**
 * NOTE:
 *
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class PRelu : public Module {
private:
    TensorBase *m_alpha;

public:
    /**
     * @brief Construct a new PRelu object.
     *
     * @param name            name of module
     * @param alpha           learnable param alpha of prelu, slope for neg part.
     * @param inplace         inplace type.
     */
    PRelu(const char *name = NULL,
          TensorBase *alpha = NULL,
          module_inplace_t inplace = MODULE_NON_INPLACE,
          quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_alpha(alpha)
    {
    }

    /**
     * @brief Destroy the PRelu object.
     */
    ~PRelu()
    {
        if (m_alpha) {
            delete m_alpha;
            m_alpha = nullptr;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        if (m_alpha->shape[0] != input_shapes[0][3]) {
            TensorBase *new_alpha = new TensorBase(
                {input_shapes[0][3], 1, 1}, nullptr, m_alpha->exponent, m_alpha->dtype, true, m_alpha->caps);
            if (m_alpha->get_dtype() == DATA_TYPE_INT16) {
                int16_t alpha_value = m_alpha->get_element<int16_t>(0);
                int16_t *alpha_ptr = new_alpha->get_element_ptr<int16_t>();
                for (int i = 0; i < input_shapes[0][3]; i++) {
                    alpha_ptr[i] = alpha_value;
                }
                delete m_alpha;
                m_alpha = new_alpha;
            }
        }
        std::vector<std::vector<int>> output_shapes(1, input_shapes[0]);
        return output_shapes;
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            forward_template<int8_t>(tensors, mode);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            forward_template<int16_t>(tensors, mode);
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "PRelu");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::prelu<int8_t>(args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::prelu<int16_t>(args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::ArgsType<T>> m_args = base::get_activation_args<T>(output, input, PReLU, m_alpha, mode);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("PRelu", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize PRelu module instance by node serialization information
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
                TensorBase *alpha = fbs_model->get_operation_parameter(node_name, 1);
                // [c, 1, 1]
                assert(alpha->shape.size() == 3);
                op = new PRelu(node_name.c_str(), alpha, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
            }
        } else {
            TensorBase *alpha = fbs_model->get_operation_parameter(node_name, 1);
            // [c, 1, 1]
            assert(alpha->shape.size() == 3);
            op = new PRelu(node_name.c_str(), alpha, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }

        return op;
    }

    void print() { ESP_LOGI("PRelU", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
