#pragma once

#include "dl_base_add.hpp"
#include "dl_base_add2d.hpp"
#include "dl_base_shape.hpp"
#include "dl_module_base.hpp"

namespace dl {
namespace module {
/**
 * @brief: Performs element-wise binary subtraction (with Numpy-style broadcasting support).
 *         Please refer to https://onnx.ai/onnx/operators/onnx__Add.html for more details
 *
 */
class Add : public Module {
private:
    std::vector<TensorBase *>
        m_inputs_constant; /*input of add. If the input of add is constant , store its TensorBase pointer; if not, pass
                          in nullptr. This container can be empty, but if values are provided, they must adhere to the
                          order and quantity defined in ONNX.*/

public:
    /**
     * @brief Construct a new Add object.
     *
     * @param name              name of module
     * @param inplace           inplace type.
     * @param quant_type        quantize type.
     * @param inputs_constant   if there are constant inputs, pass them through this parameter.
     */
    Add(const char *name = NULL,
        module_inplace_t inplace = MODULE_NON_INPLACE,
        quant_type_t quant_type = QUANT_TYPE_NONE,
        std::vector<TensorBase *> inputs_constant = {}) :
        Module(name, inplace, quant_type), m_inputs_constant(inputs_constant)
    {
    }

    /**
     * @brief Destroy the Add object.
     */
    ~Add()
    {
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

        // support multidirectional broadcasting
        std::vector<int> output_shape =
            base::get_multidirectional_broadcasting_shape(retrieve_input_shapes[0], retrieve_input_shapes[1]);

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
        // DL_LOG_LAYER_LATENCY_END(this->name, "Add2D");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::elemwise_add((base::elemwiseArgsType<int8_t> *)args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::elemwise_add((base::elemwiseArgsType<int16_t> *)args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        std::vector<TensorBase *> inputs = retrieve_inputs(tensors, m_inputs_constant);
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::elemwiseArgsType<T>> m_args =
            base::get_elemwise_operation_args<T>(output, inputs[0], inputs[1], mode);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("Add2D", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize Add module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            TensorBase *input0_constant = fbs_model->get_operation_parameter(node_name, 0);
            TensorBase *input1_constant = fbs_model->get_operation_parameter(node_name, 1);
            op = new Add(NULL, MODULE_INPLACE_CHANGED_BUFFER, quant_type, {input0_constant, input1_constant});
        }
        return op;
    }

    void print()
    {
        ESP_LOGI("Add",
                 "quant_type: %s, input feature map size: %d.",
                 quant_type_to_string(quant_type),
                 m_inputs_index.size());
    }
};

/**
 * NOTE: addition is element-wise, i.e., output[i,j,k] = input0[i,j,k] + input1[i,j,k]
 *
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class Add2D : public Module {
public:
    /**
     * @brief Construct a new Add2D object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Add2D(const char *name = NULL,
          module_inplace_t inplace = MODULE_NON_INPLACE,
          quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type)
    {
    }

    /**
     * @brief Destroy the Add2D object.
     */
    ~Add2D() {}

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
        // DL_LOG_LAYER_LATENCY_END(this->name, "Add2D");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::add2d<int8_t>(args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::add2d<int16_t>(args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input0 = tensors[m_inputs_index[0]];
        TensorBase *input1 = tensors[m_inputs_index[1]];
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::arithArgsType<T>> m_args =
            base::get_arith_operation_args<T>(output, input0, input1, Linear, nullptr, mode);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("Add2D", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize Add module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new Add2D(node_name.c_str(), MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }
        return op;
    }

    void print() { ESP_LOGI("Add2D", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
