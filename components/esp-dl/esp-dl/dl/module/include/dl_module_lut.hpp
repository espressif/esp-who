#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {
/**
 * NOTE:int16 using linear interpolation + lookup table.
 *
 * @tparam feature_t supports int16_t and int8_t,
 *         - int16_t: stands for operation in int16_t quantize
 *         - int8_t: stands for operation in int8_t quantize
 */
class LUT : public Module {
private:
    TensorBase *table; /*LUT loop up table*/
    int step;          /*LUT loop up table step: only available for int16.*/
public:
    /**
     * @brief Construct a new LUT object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    LUT(const char *name = NULL,
        TensorBase *table = NULL,
        module_inplace_t inplace = MODULE_INPLACE_CHANGED_BUFFER,
        quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type)
    {
        this->table = table;
        this->step = 1;
        if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            this->step = 65536 / (this->table->get_size() - 1);
        }
    }

    /**
     * @brief Destroy the LUT object.
     */
    ~LUT()
    {
        if (this->table) {
            delete this->table;
            this->table = nullptr;
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
        assert(output->exponent == this->table->exponent);

        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            int8_t *input_ptr = (int8_t *)input->get_element_ptr();
            int8_t *output_ptr = (int8_t *)output->get_element_ptr();
            int8_t *table_ptr = (int8_t *)(this->table->get_element_ptr());
            for (size_t i = 0; i < input->size; i++) {
                output_ptr[i] = table_ptr[input_ptr[i] + 128];
            }
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            int16_t *input_ptr = (int16_t *)input->get_element_ptr();
            int16_t *output_ptr = (int16_t *)output->get_element_ptr();
            int16_t *table_ptr = (int16_t *)(this->table->get_element_ptr());

            if (this->step == 1) {
                for (size_t i = 0; i < input->size; i++) {
                    output_ptr[i] = table_ptr[input_ptr[i] + 32768];
                }
            } else {
                for (size_t i = 0; i < input->size; i++) {
                    int idx = input_ptr[i] + 32768;
                    int len = idx % this->step;
                    idx = idx / this->step;

                    // linear interpolation
                    int x = table_ptr[idx];
                    int y = table_ptr[idx + 1];
                    output_ptr[i] = x + len * (y - x) / this->step;
                }
            }
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "LUT");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize LUT module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *table = fbs_model->get_operation_lut(node_name);

        if (table == NULL) {
            ESP_LOGE("LUT", "Table is null!");
        }

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new LUT(node_name.c_str(), table, MODULE_INPLACE_CHANGED_BUFFER, quant_type);
        } else {
            ESP_LOGE("LUT", "Only support QUANT_TYPE_SYMM_8BIT or QUANT_TYPE_SYMM_16BIT!");
        }
        return op;
    }

    void print() { ESP_LOGI("LUT", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
