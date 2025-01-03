#pragma once

#include "dl_base_resize2d.hpp"
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
class Resize2D : public Module {
private:
    const resize_mode_t resize_type; /*<! one of RESIZE_NEAREST or RESIZE_LINEAR or RESIZE_CUBIC >*/
    const float scale_y;             /*<! scale in height >*/
    const float scale_x;             /*<! scale in width >*/
public:
    /**
     * @brief Construct a new Resize2D object.
     *
     * @param name               name of module
     * @param resize_type        one of RESIZE_NEAREST or RESIZE_LINEAR or RESIZE_CUBIC
     * @param scale_y            scale in height
     * @param scale_x            scale in width
     */
    Resize2D(const char *name = NULL,
             const resize_mode_t resize_type = RESIZE_NEAREST,
             const float scale_y = 2.f,
             const float scale_x = 2.f,
             quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, MODULE_NON_INPLACE, quant_type), resize_type(resize_type), scale_y(scale_y), scale_x(scale_x)
    {
    }

    /**
     * @brief Destroy the Resize2D object.
     */
    ~Resize2D() {}

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        assert(input_shapes[0].size() == 4);
        int *input_shape = input_shapes[0].data();

        std::vector<int> output_shape(4);
        output_shape[0] = input_shape[0];
        output_shape[1] = (int)(input_shape[1] * this->scale_y);
        output_shape[2] = (int)(input_shape[2] * this->scale_x);
        output_shape[3] = input_shape[3];

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
        DL_LOG_LAYER_LATENCY_END(this->name, "Resize2D");
    }

    void forward_args(void *args)
    {
        if (quant_type == QUANT_TYPE_SYMM_8BIT) {
            base::resize2d<int8_t>(args);
        } else if (quant_type == QUANT_TYPE_SYMM_16BIT) {
            base::resize2d<int16_t>(args);
        }
    }

    template <typename T>
    void forward_template(std::vector<TensorBase *> &tensors, runtime_mode_t mode)
    {
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];

        std::vector<base::resizeArgsType<T>> m_args =
            base::get_resize_operation_args<T>(output, input, RESIZE_NEAREST, scale_y, scale_x);
        int task_size = m_args.size();
        if (task_size == 1) { // single task
            forward_args((void *)&m_args[0]);
        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
        } else {
            ESP_LOGE("Resize2D", "Only support task size is 1 or 2, currently task size is %d", task_size);
        }
    }

    /**
     * @brief deserialize Resize2D module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        resize_mode_t resize_mode;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        fbs_model->get_operation_attribute(node_name, "mode", resize_mode);
        dl::TensorBase *resize_scales_tensor = fbs_model->get_operation_parameter(node_name, 2);
        assert(resize_scales_tensor->shape.size() == 1 && resize_scales_tensor->shape[0] == 4);
        float *resize_scales = (float *)resize_scales_tensor->get_element_ptr();

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            op = new Resize2D(node_name.c_str(), resize_mode, resize_scales[2], resize_scales[3], quant_type);
        }
        delete resize_scales_tensor;
        return op;
    }

    void print() { ESP_LOGI("Resize2D", "quant_type: %s.", quant_type_to_string(quant_type)); }
};
} // namespace module
} // namespace dl
