#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Split.html
class Split : public Module {
private:
    TensorBase *m_split; /*<! Optional length of each output >*/
    int m_axis;          /*<! Which axis to split on. >*/
    int m_num_outputs;   /*<! Number of outputs to split parts of the tensor into. >*/

public:
    /**
     * @brief Construct a new Split object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Split(TensorBase *split,
          int axis = 0,
          int num_outputs = -1,
          const char *name = NULL,
          module_inplace_t inplace = MODULE_NON_INPLACE,
          quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_split(split), m_axis(axis), m_num_outputs(num_outputs)
    {
    }

    /**
     * @brief Destroy the Split object.
     */
    ~Split()
    {
        if (m_split) {
            delete m_split;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);

        if (m_axis < 0) {
            m_axis += input_shapes[0].size();
        }

        if (m_split) {
            std::vector<std::vector<int>> output_shapes(m_split->get_size(), input_shapes[0]);
            int64_t total_split_size = 0;
            int64_t *split_param = static_cast<int64_t *>(m_split->get_element_ptr());
            for (int i = 0; i < m_split->get_size(); i++) {
                total_split_size += split_param[i];
                output_shapes[i][m_axis] = static_cast<int>(split_param[i]);
            }
            assert(total_split_size == input_shapes[0][m_axis]);
            return output_shapes;
        } else {
            assert(m_num_outputs > 0);
            int output_num = (input_shapes[0][m_axis] + m_num_outputs - 1) / m_num_outputs;
            int last_output_size = input_shapes[0][m_axis] - (output_num - 1) * m_num_outputs;
            std::vector<std::vector<int>> output_shapes(output_num, input_shapes[0]);
            for (int i = 0; i < output_num - 1; i++) {
                output_shapes[i][m_axis] = m_num_outputs;
            }
            output_shapes[output_num - 1][m_axis] = last_output_size;
            return output_shapes;
        }
    }

    template <typename T>
    void forward_template(
        T *output, T *input, int slice_index, int num_slices, int slice_size, int in_axis_slice, int out_axis_slice)
    {
        for (int n = 0; n < num_slices; n++) {
            int in_offset = (n * in_axis_slice + slice_index) * slice_size;
            int out_offset = n * out_axis_slice * slice_size;
            tool::copy_memory(output + out_offset, input + in_offset, (size_t)slice_size * out_axis_slice * sizeof(T));
        }
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();

        TensorBase *input = tensors[m_inputs_index[0]];
        int num_slices = 1;
        int slice_size = 1;

        for (int i = 0; i < m_axis; i++) num_slices = num_slices * input->get_shape()[i];

        for (int i = m_axis + 1; i < input->get_shape().size(); i++) slice_size = slice_size * input->get_shape()[i];

        int output_num = m_outputs_index.size();
        int slice_index = 0;

        for (int i = 0; i < output_num; i++) {
            TensorBase *output = tensors[m_outputs_index[i]];

            if (input->get_dtype() == dl::DATA_TYPE_INT8) {
                forward_template(static_cast<int8_t *>(output->get_element_ptr()),
                                 static_cast<int8_t *>(input->get_element_ptr()),
                                 slice_index,
                                 num_slices,
                                 slice_size,
                                 input->get_shape()[m_axis],
                                 output->get_shape()[m_axis]);
            } else if (input->get_dtype() == dl::DATA_TYPE_INT16) {
                forward_template(static_cast<int16_t *>(output->get_element_ptr()),
                                 static_cast<int16_t *>(input->get_element_ptr()),
                                 slice_index,
                                 num_slices,
                                 slice_size,
                                 input->get_shape()[m_axis],
                                 output->get_shape()[m_axis]);
            } else if (input->get_dtype() == dl::DATA_TYPE_FLOAT) {
                forward_template(static_cast<float *>(output->get_element_ptr()),
                                 static_cast<float *>(input->get_element_ptr()),
                                 slice_index,
                                 num_slices,
                                 slice_size,
                                 input->get_shape()[m_axis],
                                 output->get_shape()[m_axis]);
            } else if (input->get_dtype() == dl::DATA_TYPE_DOUBLE) {
                forward_template(static_cast<double *>(output->get_element_ptr()),
                                 static_cast<double *>(input->get_element_ptr()),
                                 slice_index,
                                 num_slices,
                                 slice_size,
                                 input->get_shape()[m_axis],
                                 output->get_shape()[m_axis]);
            } else {
                ESP_LOGE("Split", "Unsupport data type.");
            }

            slice_index += output->get_shape()[m_axis];
        }

        DL_LOG_LAYER_LATENCY_END(this->name, "Split");
    }

    /**
     * @brief deserialize Split module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        int axis = 0;
        int num_outputs = -1;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "axis", axis);
        fbs_model->get_operation_attribute(node_name, "num_outputs", num_outputs);
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *split = fbs_model->get_operation_parameter(node_name, 1);

        // Create module
        op = new Split(split, axis, num_outputs, node_name.c_str(), MODULE_NON_INPLACE, quant_type);
        return op;
    }

    void print()
    {
        ESP_LOGI("Split",
                 "quant_type: %s, split shape: %s.",
                 quant_type_to_string(quant_type),
                 shape_to_string(m_split->get_shape()).c_str());
    }
};
} // namespace module
} // namespace dl
