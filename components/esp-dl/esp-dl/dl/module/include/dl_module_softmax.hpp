#pragma once

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
class Softmax : public Module {
private:
    int axis;
    float *exp_table;

public:
    /**
     * @brief Construct a new Softmax object.
     *
     * @param name            name of module
     * @param axis            the axis of the inputs. Accepted range is [-r, r-1] where r = rank(input).
     * @param inplace         inplace type.
     */
    Softmax(const char *name = NULL,
            int axis = -1,
            module_inplace_t inplace = MODULE_NON_INPLACE,
            quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), axis(axis)
    {
        this->exp_table = nullptr;
    }

    /**
     * @brief Destroy the Softmax object.
     */
    ~Softmax()
    {
        if (this->exp_table != nullptr) {
            free(this->exp_table);
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        // The output values with the same shape as the input tensor.
        std::vector<std::vector<int>> output_shapes(1, input_shapes[0]);
        return output_shapes;
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];

        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            forward_lut(input, output);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            int16_t *input_element = (int16_t *)input->get_element_ptr();
            float *output_element = (float *)output->get_element_ptr();

            float scale = DL_SCALE(input->exponent);
            for (int i = 0; i < input->get_size(); i++) {
                output_element[i] = scale * input_element[i];
            }
            forward_float(output_element, output->get_size(), output->get_shape(), this->axis);
        } else if (quant_type == QUANT_TYPE_FLOAT32) {
            float *input_element = (float *)input->get_element_ptr();
            float *output_element = (float *)output->get_element_ptr();

            if (input_element != output_element) {
                memcpy(output_element, input_element, input->get_bytes());
            }
            forward_float(output_element, output->get_size(), output->get_shape(), this->axis);
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "Softmax");
    }

    void forward_float(float *output_element, int size, std::vector<int> shape, int axis)
    {
        int dims = shape.size();
        int positive_axis = axis < 0 ? dims + axis : axis;
        int len = shape[positive_axis]; // the size of positive_axis

        if (positive_axis == dims - 1) { // positive_axis == dims - 1
            int outer_loop = 1;
            if (dims > 1) {
                outer_loop = size / len;
            }

            for (int o = 0; o < outer_loop; o++) {
                float max = output_element[0];
                for (int i = 1; i < len; i++) {
                    max = DL_MAX(max, output_element[i]);
                }

                float sum = 0.f;
                for (int i = 0; i < len; i++) {
                    output_element[i] = expf(output_element[i] - max);
                    sum += output_element[i];
                }

                for (int i = 0; i < len; i++) {
                    output_element[i] = output_element[i] / sum;
                }
                output_element += len;
            }
        } else {
            // convert input tensor to [outer_loop, len, inner_loop]
            int outer_loop = 1;
            int inner_loop = 1;
            for (int i = 0; i < dims; i++) {
                if (i < positive_axis) {
                    outer_loop *= shape[i];
                } else if (i > positive_axis) {
                    inner_loop *= shape[i];
                }
            }

            for (int o = 0; o < outer_loop; o++) {
                for (int k = 0; k < inner_loop; k++) {
                    float max = output_element[0];
                    for (int i = 1; i < len; i++) {
                        max = DL_MAX(max, output_element[i * inner_loop]);
                    }

                    float sum = 0.f;
                    for (int i = 0; i < len; i++) {
                        output_element[i * inner_loop] = expf(output_element[i * inner_loop] - max);
                        sum += output_element[i * inner_loop];
                    }

                    for (int i = 0; i < len; i++) {
                        output_element[i * inner_loop] = output_element[i * inner_loop] / sum;
                    }
                    output_element += 1;
                }
                output_element += inner_loop * (len - 1);
            }
        }
    }

    void forward_lut(TensorBase *input, TensorBase *output)
    {
        if (this->exp_table == nullptr) {
            this->exp_table = (float *)tool::calloc_aligned(256, sizeof(float), 16, MALLOC_CAP_8BIT);
            tool::gen_lut_8bit(this->exp_table, input->exponent, expf);
        }

        int dims = input->get_shape().size();
        int positive_axis = axis < 0 ? dims + axis : axis;
        int len = input->get_shape()[positive_axis]; // the size of positive_axis
        int8_t *input_element = (int8_t *)input->get_element_ptr();
        assert(output->get_dtype() == DATA_TYPE_FLOAT);
        float *output_element = (float *)output->get_element_ptr();

        if (positive_axis == dims - 1) { // positive_axis == dims - 1
            int outer_loop = 1;
            if (dims > 1) {
                outer_loop = input->get_size() / len;
            }

            for (int o = 0; o < outer_loop; o++) {
                float sum = 0.f;
                for (int i = 0; i < len; i++) {
                    output_element[i] = this->exp_table[input_element[i] + 128];
                    sum += output_element[i];
                }

                for (int i = 0; i < len; i++) {
                    output_element[i] = output_element[i] / sum;
                }
                input_element += len;
                output_element += len;
            }
        } else {
            // convert input tensor to [outer_loop, len, inner_loop]
            int outer_loop = 1;
            int inner_loop = 1;
            for (int i = 0; i < dims; i++) {
                if (i < positive_axis) {
                    outer_loop *= input->get_shape()[i];
                } else if (i > positive_axis) {
                    inner_loop *= input->get_shape()[i];
                }
            }

            for (int o = 0; o < outer_loop; o++) {
                for (int k = 0; k < inner_loop; k++) {
                    float sum = 0.f;
                    for (int i = 0; i < len; i++) {
                        output_element[i * inner_loop] = this->exp_table[input_element[i * inner_loop] + 128];
                        sum += output_element[i * inner_loop];
                    }

                    for (int i = 0; i < len; i++) {
                        output_element[i * inner_loop] = output_element[i * inner_loop] / sum;
                    }
                    input_element += 1;
                    output_element += 1;
                }
                input_element += inner_loop * (len - 1);
                output_element += inner_loop * (len - 1);
            }
        }
    }

    /**
     * @brief deserialize Softmax module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        int axis = -1;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "axis", axis);

        // Create module
        op = new Softmax(node_name.c_str(), axis, MODULE_NON_INPLACE, quant_type);
        return op;
    }

    void print() { ESP_LOGI("Softmax", "quant_type: %s. axis:%d", quant_type_to_string(quant_type), axis); }
};
} // namespace module
} // namespace dl
