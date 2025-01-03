#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Gather.html
class Gather : public Module {
private:
    TensorBase *m_indices; /*<! Gather entries of the axis dimension of data indexed by indices >*/
    int m_axis;            /*<! Which axis to gather on. >*/

public:
    /**
     * @brief Construct a new Gather object
     *
     * @param indices       Gather entries of the axis dimension of data indexed by indices
     * @param axis          Which axis to gather on.
     * @param name          Op name.
     * @param inplace       The inplace type.
     * @param quant_type    Quantization type.
     */
    Gather(TensorBase *indices,
           int axis = 0,
           const char *name = NULL,
           module_inplace_t inplace = MODULE_NON_INPLACE,
           quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_indices(indices), m_axis(axis)
    {
    }

    /**
     * @brief Destroy the Gather object.
     */
    ~Gather()
    {
        if (m_indices) {
            delete m_indices;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        assert(m_indices);

        if (m_axis < 0) {
            m_axis += input_shapes[0].size();
        }

        std::vector<int> output(m_indices->get_shape().size() + input_shapes[0].size() - 1);

        for (int in_i = 0, out_i = 0; in_i < input_shapes[0].size(); in_i++) {
            if (in_i == m_axis) {
                for (int indice_i = 0; indice_i < m_indices->get_shape().size(); indice_i++) {
                    output[out_i] = m_indices->get_shape()[indice_i];
                    out_i++;
                }
            } else {
                output[out_i] = input_shapes[0][in_i];
                out_i++;
            }
        }

        std::vector<std::vector<int>> output_shapes(1, output);
        return output_shapes;
    }

    template <typename T1, typename T2>
    void forward_template(
        T1 *output, T1 *input, T2 *indices, int outer_size, int inner_size, int indices_num, int in_axis_size)
    {
        for (int outer = 0; outer < outer_size; outer++) {
            for (int i = 0; i < indices_num; i++) {
                tool::copy_memory(output + (outer * indices_num + i) * inner_size,
                                  input + (outer * in_axis_size + static_cast<int>(indices[i])) * inner_size,
                                  inner_size * sizeof(T1));
            }
        }
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();

        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        int outer_size = 1;
        int inner_size = 1;
        int indices_num = m_indices->get_size();

        for (int i = 0; i < m_axis; i++) {
            outer_size *= input->get_shape()[i];
        }

        for (int i = m_axis + 1; i < input->get_shape().size(); i++) {
            inner_size *= input->get_shape()[i];
        }

        if (m_indices->get_dtype() == dl::DATA_TYPE_INT64) {
            if (input->get_dtype() == dl::DATA_TYPE_INT8) {
                forward_template(static_cast<int8_t *>(output->get_element_ptr()),
                                 static_cast<int8_t *>(input->get_element_ptr()),
                                 static_cast<int64_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_INT16) {
                forward_template(static_cast<int16_t *>(output->get_element_ptr()),
                                 static_cast<int16_t *>(input->get_element_ptr()),
                                 static_cast<int64_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_FLOAT) {
                forward_template(static_cast<float *>(output->get_element_ptr()),
                                 static_cast<float *>(input->get_element_ptr()),
                                 static_cast<int64_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_DOUBLE) {
                forward_template(static_cast<double *>(output->get_element_ptr()),
                                 static_cast<double *>(input->get_element_ptr()),
                                 static_cast<int64_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);
            }
        } else if (m_indices->get_dtype() == dl::DATA_TYPE_INT32) {
            if (input->get_dtype() == dl::DATA_TYPE_INT8) {
                forward_template(static_cast<int8_t *>(output->get_element_ptr()),
                                 static_cast<int8_t *>(input->get_element_ptr()),
                                 static_cast<int32_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_INT16) {
                forward_template(static_cast<int16_t *>(output->get_element_ptr()),
                                 static_cast<int16_t *>(input->get_element_ptr()),
                                 static_cast<int32_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_FLOAT) {
                forward_template(static_cast<float *>(output->get_element_ptr()),
                                 static_cast<float *>(input->get_element_ptr()),
                                 static_cast<int32_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);

            } else if (input->get_dtype() == dl::DATA_TYPE_DOUBLE) {
                forward_template(static_cast<double *>(output->get_element_ptr()),
                                 static_cast<double *>(input->get_element_ptr()),
                                 static_cast<int32_t *>(m_indices->get_element_ptr()),
                                 outer_size,
                                 inner_size,
                                 indices_num,
                                 input->get_shape()[m_axis]);
            }
        }

        DL_LOG_LAYER_LATENCY_END(this->name, "Gather");
    }

    /**
     * @brief deserialize Gather module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        int axis = 0;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "axis", axis);
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *indices = fbs_model->get_operation_parameter(node_name, 1);

        // Create module
        op = new Gather(indices, axis, node_name.c_str(), MODULE_NON_INPLACE, quant_type);
        return op;
    }

    void print()
    {
        ESP_LOGI("Gather",
                 "quant_type: %s, indices shape: %s.",
                 quant_type_to_string(quant_type),
                 shape_to_string(m_indices->get_shape()).c_str());
    }
};
} // namespace module
} // namespace dl
