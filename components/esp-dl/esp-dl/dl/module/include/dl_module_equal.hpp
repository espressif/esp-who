#pragma once

#include "dl_base_elemwise.hpp"
#include "dl_base_equal.hpp"
#include "dl_base_shape.hpp"
#include "dl_module_base.hpp"

namespace dl {
namespace module {

class Equal : public Module {
public:
    /**
     * @brief Construct a new Equal2D object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Equal(const char *name = NULL,
          module_inplace_t inplace = MODULE_NON_INPLACE,
          quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type)
    {
    }

    /**
     * @brief Destroy the Equal2D object.
     */
    ~Equal() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 2);

        // support multidirectional broadcasting
        std::vector<int> output_shape = base::get_multidirectional_broadcasting_shape(input_shapes[0], input_shapes[1]);

        return std::vector<std::vector<int>>(1, output_shape);
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        // DL_LOG_LAYER_LATENCY_INIT();
        // DL_LOG_LAYER_LATENCY_START();
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            forward_template<int8_t>(tensors, mode);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            forward_template<int16_t>(tensors, mode);
        }
        // DL_LOG_LAYER_LATENCY_END(this->name, "Equal2D");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::elemwise_equal((base::elemwiseArgsType<int8_t> *)args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::elemwise_equal((base::elemwiseArgsType<int16_t> *)args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input0 = tensors[m_inputs_index[0]];
        TensorBase *input1 = tensors[m_inputs_index[1]];
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::elemwiseArgsType<T>> m_args =
            base::get_elemwise_operation_args<T>(output, input0, input1, mode);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("Equal2D", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize Equal module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new Equal(node_name.c_str(), MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }
        return op;
    }

    void print() { ESP_LOGI("Equal", "quant_type: %s.", quant_type_to_string(quant_type)); }
};

} // namespace module
} // namespace dl
