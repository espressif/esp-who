#pragma once

#include "dl_base_avg_pool2d.hpp"
#include "dl_module_base.hpp"

namespace dl {
namespace module {
/**
 * NOTE:
 *
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class GlobalAveragePool2D : public Module {
public:
    /**
     * @brief Construct a new GlobalAveragePool2D object.
     *
     * @param name            name of module
     */
    GlobalAveragePool2D(const char *name = NULL, quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, MODULE_NON_INPLACE, quant_type)
    {
    }

    /**
     * @brief Destroy the GlobalAveragePool2D object.
     */
    GlobalAveragePool2D() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        assert(input_shapes[0].size() == 4);
        int *input_shape = input_shapes[0].data();

        std::vector<int> output_shape(4, 1);
        output_shape[3] = input_shape[3];

        std::vector<std::vector<int>> output_shapes(1, output_shape);
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
        DL_LOG_LAYER_LATENCY_END(this->name, "GlobalAveragePool2D");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::avg_pool2d<int8_t>(args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::avg_pool2d<int16_t>(args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::PoolArgsType<T>> m_args =
            base::get_pool_args<T>(output, input, {0, 0, 0, 0}, {input->shape[1], input->shape[2]}, 1, 1, mode);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("GlobalAveragePool2D", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize GlobalAveragePool2D module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new GlobalAveragePool2D(node_name.c_str(), quant_type);
        }
        return op;
    }

    void print() { ESP_LOGI("GlobalAveragePool2D", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
