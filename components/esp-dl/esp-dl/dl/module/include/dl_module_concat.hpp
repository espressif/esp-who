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
class Concat : public Module {
private:
    int axis;     /*<! axis to concat >*/
    int n_dims;   /*<! num dimensions >*/
    int n_inputs; /*<! num inputs >*/
    int loop_times;
    std::vector<int> copy_nums;

public:
    /**
     * @brief Construct a new Concat object.
     *
     * @param name            name of module
     */
    Concat(const char *name = NULL, int axis = 0, quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, MODULE_NON_INPLACE, quant_type), axis(axis)
    {
    }

    /**
     * @brief Destroy the Concat object.
     */
    ~Concat() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        this->n_inputs = input_shapes.size();
        assert(this->n_inputs > 1);
        this->n_dims = input_shapes[0].size();

        if (this->axis < 0)
            this->axis += this->n_dims;
        assert(this->axis >= 0 && this->axis < this->n_dims);

        int output_axis_dim = 0;
        this->loop_times = 1;
        this->copy_nums.assign(this->n_inputs, 1);
        for (size_t i = 0; i < this->n_inputs; i++) {
            assert(input_shapes[i].size() == this->n_dims);
            for (size_t j = 0; j < this->n_dims; j++) {
                if (i == 0 && j < this->axis) {
                    this->loop_times *= input_shapes[0][j];
                }
                if (i > 0 && j != this->axis) {
                    assert(input_shapes[i][j] == input_shapes[i - 1][j]);
                }
                if (j >= this->axis) {
                    this->copy_nums[i] *= input_shapes[i][j];
                }
            }
            output_axis_dim += input_shapes[i][this->axis];
        }

        std::vector<int> output_shape(input_shapes[0]);
        output_shape[this->axis] = output_axis_dim;
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
        DL_LOG_LAYER_LATENCY_END(this->name, "Concat");
    }

    void forward_args(void *args) {}

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *output = tensors[m_outputs_index[0]];
        T *output_ptr = (T *)output->get_element_ptr();

        std::vector<T *> inputs_ptr(this->n_inputs);
        for (size_t i = 0; i < this->n_inputs; i++) {
            TensorBase *input = tensors[m_inputs_index[i]];
            inputs_ptr[i] = (T *)input->get_element_ptr();
        }

        for (size_t i = 0; i < this->loop_times; i++) {
            for (size_t j = 0; j < this->n_inputs; j++) {
                tool::copy_memory(output_ptr, inputs_ptr[j], sizeof(T) * this->copy_nums[j]);
                output_ptr += copy_nums[j];
                inputs_ptr[j] += copy_nums[j];
            }
        }
    }

    /**
     * @brief deserialize Concat module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        int axis;
        std::vector<int> output_shape;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "axis", axis);
        fbs_model->get_operation_output_shape(node_name, 0, output_shape);
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // if (output_shape.size() == 4)
        // {
        //     assert (axis > 0 && axis < 4);
        //     // n c h w => h w c
        //     switch (axis){
        //         case 1: axis = 2;
        //                 break;
        //         case 2: axis = 0;
        //                 break;
        //         case 3: axis = 1;
        //                 break;
        //     }
        // }
        // else if (output_shape.size() == 3)
        // {
        //     assert (axis > 0 && axis < 3);
        //     // n c w => w c
        //     switch (axis){
        //         case 1: axis = 1;
        //                 break;
        //         case 2: axis = 0;
        //                 break;
        //     }
        // }
        // else if (output_shape.size() == 2)
        // {
        //     // n c => c
        //     assert (axis == 1);
        //     axis = 0;
        // }

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new Concat(node_name.c_str(), axis, quant_type);
        }
        return op;
    }

    void print() { ESP_LOGI("Concat", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
