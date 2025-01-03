#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Slice.html
class Slice : public Module {
private:
    std::vector<int> m_start; /*<! starting indices >*/
    std::vector<int> m_end;   /*<! ending indices >*/
    std::vector<int> m_axes;  /*<! axes that starts and ends apply to >*/
    std::vector<int> m_step;  /*<! slice step >*/

public:
    /**
     * @brief Construct a new Slice object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Slice(std::vector<int> start,
          std::vector<int> end,
          std::vector<int> axes = {},
          std::vector<int> step = {},
          const char *name = NULL,
          module_inplace_t inplace = MODULE_NON_INPLACE,
          quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_start(start), m_end(end), m_axes(axes), m_step(step)
    {
    }

    /**
     * @brief Destroy the Slice object.
     */
    ~Slice() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        std::vector<int> output_shape = base::get_slice_shape(input_shapes[0], m_start, m_end, m_axes, m_step);

        if (output_shape.empty()) {
            ESP_LOGE("Slice", "output shape is empty!");
            assert(false);
        }
        return std::vector<std::vector<int>>(1, output_shape);
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];

        output->slice(input, m_start, m_end, m_axes, m_step);
        DL_LOG_LAYER_LATENCY_END(this->name, "Slice");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Slice module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *start = fbs_model->get_operation_parameter(node_name, 1);
        TensorBase *end = fbs_model->get_operation_parameter(node_name, 2);
        TensorBase *axes = fbs_model->get_operation_parameter(node_name, 3);
        TensorBase *step = fbs_model->get_operation_parameter(node_name, 4);

        // convert from TensorBase to vector<int>
        // ONNX Slice support int32_t and int64_t index, but we only support int32_t
        int dims = start->get_size();
        std::vector<int> start_index(dims, 0);
        std::vector<int> end_index(dims, 0);
        std::vector<int> axes_index(dims, 0);
        std::vector<int> step_index(dims, 0);
        if (start->get_dtype() == DATA_TYPE_INT32) {
            for (int i = 0; i < dims; i++) {
                start_index[i] = start->get_element<int32_t>(i);
                end_index[i] = end->get_element<int32_t>(i);
                if (axes != nullptr) {
                    axes_index[i] = axes->get_element<int32_t>(i);
                }
                if (step != nullptr) {
                    step_index[i] = step->get_element<int32_t>(i);
                }
            }
        } else if (start->get_dtype() == DATA_TYPE_INT64) {
            for (int i = 0; i < dims; i++) {
                start_index[i] = (int)start->get_element<int64_t>(i);
                end_index[i] = (int)end->get_element<int64_t>(i);
                if (axes != nullptr) {
                    axes_index[i] = (int)axes->get_element<int64_t>(i);
                }
                if (step != nullptr) {
                    step_index[i] = (int)step->get_element<int64_t>(i);
                }
            }
        }
        if (axes == nullptr) {
            axes_index.clear();
        }
        if (step == nullptr) {
            step_index.clear();
        }

        // Create module
        op = new Slice(
            start_index, end_index, axes_index, step_index, node_name.c_str(), MODULE_NON_INPLACE, quant_type);

        delete start;
        delete end;
        if (axes != nullptr) {
            delete axes;
        }
        if (step != nullptr) {
            delete step;
        }
        return op;
    }

    void print() { ESP_LOGI("Slice", "quant_type: %s", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
