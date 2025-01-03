#include "dl_module_base.hpp"
#include "dl_module_lut.hpp"
namespace dl {
namespace module {

/**
 * @brief: Please refer to https://onnx.ai/onnx/operators/onnx__Clip.html for more details
 *
 * @tparam feature_t supports float, int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class Clip : public Module {
private:
    TensorBase *m_min; /*<! Minimum value of Clip >*/
    TensorBase *m_max; /*<! Maximum value of Clip >*/

public:
    /**
     * @brief Construct a new Clip object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Clip(TensorBase *min,
         TensorBase *max,
         const char *name = NULL,
         module_inplace_t inplace = MODULE_NON_INPLACE,
         quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_min(min), m_max(max)
    {
    }

    /**
     * @brief Destroy the Clip object.
     */
    ~Clip()
    {
        if (m_min) {
            delete m_min;
            m_min = nullptr;
        }

        if (m_max) {
            delete m_max;
            m_max = nullptr;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
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
            int8_t *input_ptr = (int8_t *)input->get_element_ptr();
            int8_t *output_ptr = (int8_t *)output->get_element_ptr();
            int8_t min_value = m_min->get_element<int8_t>(0);
            int8_t max_value = m_max->get_element<int8_t>(0);

            float rescale = DL_SCALE(input->exponent) * DL_RESCALE(output->exponent);
            for (size_t i = 0; i < input->size; i++) {
                int8_t temp = DL_CLIP(input_ptr[i], min_value, max_value);
                tool::truncate(output_ptr[i], tool::round(temp * rescale));
            }
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            int16_t *input_ptr = input->get_element_ptr<int16_t>();
            int16_t *output_ptr = output->get_element_ptr<int16_t>();
            int16_t min_value = m_min->get_element<int16_t>(0);
            int16_t max_value = m_max->get_element<int16_t>(0);

            float rescale = DL_SCALE(input->exponent) * DL_RESCALE(output->exponent);
            for (size_t i = 0; i < input->size; i++) {
                int16_t temp = DL_CLIP(input_ptr[i], min_value, max_value);
                tool::truncate(output_ptr[i], tool::round(temp * rescale));
            }
        } else if (quant_type == QUANT_TYPE_FLOAT32) {
            float *input_ptr = input->get_element_ptr<float>();
            float *output_ptr = output->get_element_ptr<float>();
            float min_value = m_min->get_element<float>(0);
            float max_value = m_max->get_element<float>(0);

            for (size_t i = 0; i < input->size; i++) {
                output_ptr[i] = DL_CLIP(input_ptr[i], min_value, max_value);
            }
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "Clip");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Clip module instance by node serialization information
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
            }
        }
        if (op == nullptr) {
            TensorBase *min = fbs_model->get_operation_parameter(node_name, 1);
            TensorBase *max = fbs_model->get_operation_parameter(node_name, 2);
            assert(min->exponent == max->exponent);
            op = new Clip(min, max, node_name.c_str(), MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        }
        return op;
    }

    void print() { ESP_LOGI("Clip", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
