#pragma once

#include "dl_base_conv2d.hpp"
#include "dl_base_depthwise_conv2d.hpp"
#include "dl_module_base.hpp"
#include <typeinfo>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace dl {
namespace module {

/**
 * @brief Activation(Gemm(input, filter) + bias).
 *
 */
class Gemm : public Module {
private:
    TensorBase *filter;           /*<! filter of Gemm. It's shape is [1, 1, in_features, out_features] >*/
    TensorBase *bias;             /*<! bias of Gemm, if you don't specify anything, no bias is added >*/
    activation_type_t activation; /*<! activation of Gemm, if you don't specify anything, no activation is applied >*/

public:
    /**
     * @brief Construct a new Gemm object.
     *
     * @param filter          filter of Gemm. It's shape is [1, 1, in_features, out_features]
     * @param bias            bias of Gemm, if you don't specify anything, no bias is added
     * @param activation      activation of Gemm, if you don't specify anything, no activation is applied
     * @param name            name of module
     */
    Gemm(TensorBase *filter,
         TensorBase *bias = nullptr,
         activation_type_t activation = Linear,
         const char *name = nullptr,
         quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, MODULE_NON_INPLACE, quant_type), filter(filter), bias(bias), activation(activation)
    {
    }

    /**
     * @brief Destroy the Gemm object.
     *
     */
    ~Gemm()
    {
        if (filter) {
            delete filter;
            filter = nullptr;
        }

        if (bias) {
            delete bias;
            bias = nullptr;
        }
    }

    /**
     * @brief Calculate the output shape
     *
     * @param input_shape The shape of inputs
     *
     * @return output shape
     */
    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        assert(filter->shape.size() == 4);
        assert(filter->shape[0] == 1);
        assert(filter->shape[1] == 1);
        assert(input_shapes[0][input_shapes[0].size() - 1] == filter->shape[2]);

        // refer to https://pytorch.org/docs/stable/generated/torch.nn.Linear.html
        std::vector<int> output_shape = input_shapes[0];
        output_shape[output_shape.size() - 1] = filter->shape[3];
        std::vector<std::vector<int>> output_shapes(1, output_shape);
        return output_shapes;
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::conv2d<int8_t, int32_t, int32_t>(args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::conv2d<int16_t, int32_t, int64_t>(args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        std::vector<int> padding(4, 0);
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        std::vector<int> origin_input_shape = input->get_shape();
        std::vector<int> origin_output_shape = output->get_shape();
        input->set_shape({1, 1, input->get_size() / origin_input_shape.back(), origin_input_shape.back()});
        output->set_shape({1, 1, output->get_size() / origin_output_shape.back(), origin_output_shape.back()});

        std::vector<base::ArgsType<T>> m_args =
            base::get_conv_operation_args<T>(output,
                                             input,
                                             padding,
                                             this->filter,
                                             1 /*stride_y*/,
                                             1 /*stride_x*/,
                                             1 /*dilation_y*/,
                                             1 /*dilation_x*/,
                                             1 /*group*/,
                                             this->bias,
                                             this->activation,
                                             nullptr,
                                             mode); // do not support PReLU and Leaky RelU
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            ESP_LOGI("Gemm", "two task...");
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("Gemm", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
        input->set_shape(origin_input_shape);
        output->set_shape(origin_output_shape);
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
        DL_LOG_LAYER_LATENCY_END(this->name, "Gemm");
    }

    /**
     * @brief deserialize Conv2d module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *gemm_op = nullptr;

        activation_type_t activation_type;
        quant_type_t quant_type;
        int transA = -1, transB = -1;
        fbs_model->get_operation_attribute(node_name, "activation", activation_type);
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "transA", transA);
        fbs_model->get_operation_attribute(node_name, "transB", transB);
        assert(transA == -1 || transA == 0);
        assert(transB == -1 || transB == 0);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            TensorBase *filter = fbs_model->get_operation_parameter(node_name, 1);
            TensorBase *bias = fbs_model->get_operation_parameter(node_name, 2);
            if (bias) {
                bias->reset_bias_layout(quant_type, false);
            }

            gemm_op = new Gemm(filter, bias, activation_type, node_name.c_str(), quant_type);
        }

        return gemm_op;
    }

    void print()
    {
        ESP_LOGI("Gemm",
                 "filter:%s, bias:%s, activation: %s, "
                 "quant_type: %s.",
                 shape_to_string(filter->shape).c_str(),
                 bias == nullptr ? "false" : "true",
                 activation_type_to_string(activation),
                 quant_type_to_string(quant_type));
    }
};
} // namespace module
} // namespace dl
