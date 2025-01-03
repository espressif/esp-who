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
 * @brief Activation(MatMul(input0, input1)).
 *
 */
class MatMul : public Module {
private:
    TensorBase *m_filter; /*<! filter of MatMul. If matmul has a constant input, the value is not NULL; otherwise, it is
                             NULL >*/
    activation_type_t
        m_activation; /*<! activation of MatMul, if you don't specify anything, no activation is applied >*/

public:
    /**
     * @brief Construct a new MatMul object.
     *
     * @param activation      activation of MatMul, if you don't specify anything, no activation is applied
     * @param name            name of module
     */
    MatMul(TensorBase *filter,
           activation_type_t activation = Linear,
           const char *name = nullptr,
           quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, MODULE_NON_INPLACE, quant_type), m_filter(filter), m_activation(activation)
    {
    }

    /**
     * @brief Destroy the MatMul object.
     *
     */
    ~MatMul()
    {
        if (m_filter) {
            delete m_filter;
        }
    }

    /**
     * @brief Calculate the output shape
     *
     * @param input_shapes The shape of inputs
     *
     * @return output shapes
     */
    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1 || input_shapes.size() == 2);

        std::vector<int> input1_shape;
        if (input_shapes.size() == 1) {
            assert(m_filter);
            input1_shape = m_filter->get_shape();
        } else {
            input1_shape = input_shapes[1];
        }

        // refer to https://pytorch.org/docs/stable/generated/torch.matmul.html#torch-matmul
        std::vector<int> output_shape;
        if (input_shapes[0].size() == 1 && input1_shape.size() == 1) {
            assert(input_shapes[0][0] == input1_shape[0]);
            output_shape.push_back(1);

        } else if (input_shapes[0].size() == 2 && input1_shape.size() == 2) {
            assert(input_shapes[0][1] == input1_shape[0]);
            output_shape = {input_shapes[0][0], input1_shape[1]};

        } else if (input_shapes[0].size() == 1 && input1_shape.size() == 2) {
            assert(input_shapes[0][0] == input1_shape[0]);
            output_shape = {input1_shape[1]};

        } else if (input_shapes[0].size() == 2 && input1_shape.size() == 1) {
            assert(input_shapes[0][1] == input1_shape[0]);
            output_shape = {input_shapes[0][0]};

        } else if (input_shapes[0].size() == 1 && input1_shape.size() > 2) {
            assert(input_shapes[0][0] == input1_shape[input1_shape.size() - 2]);
            output_shape.assign(input1_shape.begin(), input1_shape.begin() + input1_shape.size() - 2);
            output_shape.push_back(input1_shape[input1_shape.size() - 1]);

        } else if (input_shapes[0].size() > 2 && input1_shape.size() == 1) {
            assert(input_shapes[0].back() == input1_shape[0]);
            output_shape.assign(input_shapes[0].begin(), input_shapes[0].begin() + input_shapes[0].size() - 1);

        } else if (std::max(input_shapes[0].size(), input1_shape.size()) == 3) {
            assert(input_shapes[0].back() == input1_shape[input1_shape.size() - 2]);
            int input0_batch = input_shapes[0].size() == 2 ? 1 : input_shapes[0][0];
            int input1_batch = input1_shape.size() == 2 ? 1 : input1_shape[0];
            assert(input0_batch == 1 || input1_batch == 1 || input0_batch == input1_batch);
            output_shape = {
                std::max(input0_batch, input1_batch), input_shapes[0][input_shapes[0].size() - 2], input1_shape.back()};

        } else if (std::max(input_shapes[0].size(), input1_shape.size()) == 4) {
            assert(input_shapes[0].back() == input1_shape[input1_shape.size() - 2]);
            int input0_batch0 = input_shapes[0].size() == 2 ? 1 : input_shapes[0].size() == 3 ? 1 : input_shapes[0][0];
            int input1_batch0 = input1_shape.size() == 2 ? 1 : input1_shape.size() == 3 ? 1 : input1_shape[0];
            assert(input0_batch0 == 1 || input1_batch0 == 1 || input0_batch0 == input1_batch0);

            int input0_batch1 = input_shapes[0].size() == 2 ? 1
                : input_shapes[0].size() == 3               ? input_shapes[0][0]
                                                            : input_shapes[0][1];
            int input1_batch1 = input1_shape.size() == 2 ? 1
                : input1_shape.size() == 3               ? input1_shape[0]
                                                         : input1_shape[1];
            assert(input0_batch1 == 1 || input1_batch1 == 1 || input0_batch1 == input1_batch1);

            output_shape = {std::max(input0_batch0, input1_batch0),
                            std::max(input0_batch1, input1_batch1),
                            input_shapes[0][input_shapes[0].size() - 2],
                            input1_shape.back()};

        } else {
            ESP_LOGE("MatMul",
                     "Impossible matmul, input0 dims: %d, input1 dims: %d",
                     input_shapes[0].size(),
                     input1_shape.size());
        }
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
        TensorBase *input0 = tensors[m_inputs_index[0]];
        TensorBase *input1 = nullptr;
        TensorBase *output = tensors[m_outputs_index[0]];
        std::vector<int> origin_input0_shape = input0->get_shape();
        std::vector<int> origin_input1_shape;
        std::vector<int> origin_output_shape = output->get_shape();

        if (m_inputs_index.size() == 1) {
            input1 = m_filter;
            origin_input1_shape = m_filter->get_shape();
        } else {
            input1 = tensors[m_inputs_index[1]];
            origin_input1_shape = input1->get_shape();
        }

        // input: MK -> NHWC; filter: KN -> HWIO; output: MN -> NHWC
        if (origin_input0_shape.size() <= 2 && origin_input1_shape.size() <= 2) {
            if (origin_input0_shape.size() == 1 && origin_input1_shape.size() == 1) {
                // dot product
                // input: NHWC
                input0->set_shape({1, 1, 1, origin_input0_shape[0]});
                // filter: HWIO
                input1->set_shape({1, 1, origin_input1_shape[0], 1});
                // output: NHWC
                output->set_shape({1, 1, 1, 1});

            } else if (origin_input0_shape.size() == 2 && origin_input1_shape.size() == 2) {
                // matrix multiply
                // input: NHWC
                input0->set_shape({1, 1, origin_input0_shape[0], origin_input0_shape[1]});
                // filter: HWIO
                input1->set_shape({1, 1, origin_input1_shape[0], origin_input1_shape[1]});
                // output: NHWC
                output->set_shape({1, 1, origin_output_shape[0], origin_output_shape[1]});

            } else if (origin_input0_shape.size() == 1 && origin_input1_shape.size() == 2) {
                // input: NHWC
                input0->set_shape({1, 1, 1, origin_input0_shape[0]});
                // filter: HWIO
                input1->set_shape({1, 1, origin_input1_shape[0], origin_input1_shape[1]});
                // output: NHWC
                output->set_shape({1, 1, 1, origin_output_shape[0]});

            } else if (origin_input0_shape.size() == 2 && origin_input1_shape.size() == 1) {
                // input: NHWC
                input0->set_shape({1, 1, origin_input0_shape[0], origin_input0_shape[1]});
                // filter: HWIO
                input1->set_shape({1, 1, origin_input1_shape[0], 1});
                // output: NHWC
                output->set_shape({1, 1, origin_output_shape[0], 1});
            }

            std::vector<base::ArgsType<T>> m_args =
                base::get_conv_operation_args<T>(output,
                                                 input0,
                                                 padding,
                                                 input1 /*filter*/,
                                                 1 /*stride_y*/,
                                                 1 /*stride_x*/,
                                                 1 /*dilation_y*/,
                                                 1 /*dilation_x*/,
                                                 1 /*group*/,
                                                 nullptr /*bias*/,
                                                 m_activation,
                                                 nullptr,
                                                 mode); // do not support PReLU and Leaky RelU
            int task_size = m_args.size();
            if (task_size == 1) { // single task
                forward_args((void *)&m_args[0]);
            } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
                ESP_LOGI("MatMul", "two task...");
                module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
            } else {
                ESP_LOGE("MatMul", "Only support task size is 1 or 2, currently task size is %d", task_size);
            }

        } else {
            // batched matrix multiply
            size_t input0_dtype_bytes = input0->get_dtype_bytes();
            void *input0_element = input0->get_element_ptr();
            size_t input1_dtype_bytes = input1->get_dtype_bytes();
            void *input1_element = input1->get_element_ptr();
            size_t output_dtype_bytes = output->get_dtype_bytes();
            void *output_element = output->get_element_ptr();

            if (origin_input0_shape.size() == 1 && origin_input1_shape.size() > 2) {
                int input1_batch_size = 1;
                for (int i = 0; i < origin_input1_shape.size() - 2; i++) {
                    input1_batch_size *= origin_input1_shape[i];
                }

                int c = origin_input1_shape[origin_input1_shape.size() - 2];
                int n = origin_input1_shape.back();
                int align = input1->get_dtype() == DATA_TYPE_INT8 ? 16 : 8;
                bool is_align = (c * n % align) == 0;
                input1->set_shape({input1_batch_size, c, n});

                // input: NHWC
                input0->set_shape({1, 1, 1, origin_input0_shape.back()});
                // output: NHWC
                output->set_shape({input1_batch_size, origin_output_shape.back()});

                TensorBase input1_tmp({1, 1, c, n} /*shape*/,
                                      input1_element /*element*/,
                                      input1->exponent /*exponent*/,
                                      input1->dtype /*dtype*/,
                                      is_align ? false : true /*deep*/,
                                      input1->caps /*caps*/);

                for (int i = 0; i < input1_batch_size; i++) {
                    // filter: HWIO
                    if (!is_align) {
                        input1_tmp.assign({1, 1, c, n} /*shape*/,
                                          static_cast<char *>(input1_element) +
                                              input1_dtype_bytes * input1->get_element_index({i, 0, 0}) /*element*/,
                                          input1->exponent /*exponent*/,
                                          input1->dtype /*dtype*/);
                    } else {
                        input1_tmp.set_element(static_cast<char *>(input1_element) +
                                               input1_dtype_bytes * input1->get_element_index({i, 0, 0}));
                    }

                    // output: NHWC
                    TensorBase output_tmp({1, 1, 1, origin_output_shape.back()} /*shape*/,
                                          static_cast<char *>(output_element) +
                                              output_dtype_bytes * output->get_element_index({i, 0}) /*element*/,
                                          output->exponent /*exponent*/,
                                          output->dtype /*dtype*/,
                                          false /*deep*/,
                                          output->caps /*caps*/);

                    std::vector<base::ArgsType<T>> m_args =
                        base::get_conv_operation_args<T>(&output_tmp,
                                                         input0,
                                                         padding,
                                                         &input1_tmp /*filter*/,
                                                         1 /*stride_y*/,
                                                         1 /*stride_x*/,
                                                         1 /*dilation_y*/,
                                                         1 /*dilation_x*/,
                                                         1 /*group*/,
                                                         nullptr /*bias*/,
                                                         m_activation,
                                                         nullptr,
                                                         mode); // do not support PReLU and Leaky RelU
                    int task_size = m_args.size();
                    if (task_size == 1) { // single task
                        forward_args((void *)&m_args[0]);
                    } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
                        ESP_LOGI("MatMul", "two task...");
                        module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
                    } else {
                        ESP_LOGE("MatMul", "Only support task size is 1 or 2, currently task size is %d", task_size);
                    }
                }

            } else if (origin_input0_shape.size() > 2 && origin_input1_shape.size() == 1) {
                int input0_batch_size = 1;
                for (int i = 0; i < origin_input0_shape.size() - 2; i++) {
                    input0_batch_size *= origin_input0_shape[i];
                }
                input0->set_shape({input0_batch_size,
                                   origin_input0_shape[origin_input0_shape.size() - 2],
                                   origin_input0_shape.back()});
                // filter: HWIO
                input1->set_shape({1, 1, origin_input1_shape.back(), 1});
                // output: NHWC
                output->set_shape({input0_batch_size, origin_output_shape.back()});

                for (int i = 0; i < input0_batch_size; i++) {
                    // input: NHWC
                    TensorBase input0_tmp({1,
                                           1,
                                           origin_input0_shape[origin_input0_shape.size() - 2],
                                           origin_input0_shape.back()} /*shape*/,
                                          static_cast<char *>(input0_element) +
                                              input0_dtype_bytes * input0->get_element_index({i, 0, 0}) /*element*/,
                                          input0->exponent /*exponent*/,
                                          input0->dtype /*dtype*/,
                                          false /*deep*/,
                                          input0->caps /*caps*/);
                    // output: NHWC
                    TensorBase output_tmp({1, 1, origin_output_shape.back(), 1} /*shape*/,
                                          static_cast<char *>(output_element) +
                                              output_dtype_bytes * output->get_element_index({i, 0}) /*element*/,
                                          output->exponent /*exponent*/,
                                          output->dtype /*dtype*/,
                                          false /*deep*/,
                                          output->caps /*caps*/);

                    std::vector<base::ArgsType<T>> m_args =
                        base::get_conv_operation_args<T>(&output_tmp,
                                                         &input0_tmp,
                                                         padding,
                                                         input1 /*filter*/,
                                                         1 /*stride_y*/,
                                                         1 /*stride_x*/,
                                                         1 /*dilation_y*/,
                                                         1 /*dilation_x*/,
                                                         1 /*group*/,
                                                         nullptr /*bias*/,
                                                         m_activation,
                                                         nullptr,
                                                         mode); // do not support PReLU and Leaky RelU
                    int task_size = m_args.size();
                    if (task_size == 1) { // single task
                        forward_args((void *)&m_args[0]);
                    } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
                        ESP_LOGI("MatMul", "two task...");
                        module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
                    } else {
                        ESP_LOGE("MatMul", "Only support task size is 1 or 2, currently task size is %d", task_size);
                    }
                }

            } else if (std::max(origin_input0_shape.size(), origin_input1_shape.size()) == 3) {
                int input0_batch = origin_input0_shape.size() == 2 ? 1 : origin_input0_shape[0];
                int input1_batch = origin_input1_shape.size() == 2 ? 1 : origin_input1_shape[0];

                int c = origin_input1_shape[origin_input1_shape.size() - 2];
                int n = origin_input1_shape.back();
                int align = input1->get_dtype() == DATA_TYPE_INT8 ? 16 : 8;
                bool is_align = (c * n % align) == 0;
                input0->set_shape(
                    {input0_batch, origin_input0_shape[origin_input0_shape.size() - 2], origin_input0_shape.back()});
                input1->set_shape({input1_batch, c, n});
                int max_batch = std::max(input0_batch, input1_batch);

                TensorBase input1_tmp({1, 1, c, n} /*shape*/,
                                      input1_element /*element*/,
                                      input1->exponent /*exponent*/,
                                      input1->dtype /*dtype*/,
                                      is_align ? false : true /*deep*/,
                                      input1->caps /*caps*/);

                for (int i = 0; i < max_batch; i++) {
                    // input: NHWC
                    int input0_i = input0_batch == 1 ? 0 : i;
                    TensorBase input0_tmp({1,
                                           1,
                                           origin_input0_shape[origin_input0_shape.size() - 2],
                                           origin_input0_shape.back()} /*shape*/,
                                          static_cast<char *>(input0_element) +
                                              input0_dtype_bytes *
                                                  input0->get_element_index({input0_i, 0, 0}) /*element*/,
                                          input0->exponent /*exponent*/,
                                          input0->dtype /*dtype*/,
                                          false /*deep*/,
                                          input0->caps /*caps*/);

                    // filter: HWIO
                    int input1_i = input1_batch == 1 ? 0 : i;
                    if (!is_align) {
                        input1_tmp.assign({1, 1, c, n} /*shape*/,
                                          static_cast<char *>(input1_element) +
                                              input1_dtype_bytes *
                                                  input1->get_element_index({input1_i, 0, 0}) /*element*/,
                                          input1->exponent /*exponent*/,
                                          input1->dtype /*dtype*/);
                    } else {
                        input1_tmp.set_element(static_cast<char *>(input1_element) +
                                               input1_dtype_bytes * input1->get_element_index({input1_i, 0, 0}));
                    }

                    // output: NHWC
                    TensorBase output_tmp({1,
                                           1,
                                           origin_output_shape[origin_output_shape.size() - 2],
                                           origin_output_shape.back()} /*shape*/,
                                          static_cast<char *>(output_element) +
                                              output_dtype_bytes * output->get_element_index({i, 0, 0}) /*element*/,
                                          output->exponent /*exponent*/,
                                          output->dtype /*dtype*/,
                                          false /*deep*/,
                                          output->caps /*caps*/);

                    std::vector<base::ArgsType<T>> m_args =
                        base::get_conv_operation_args<T>(&output_tmp,
                                                         &input0_tmp,
                                                         padding,
                                                         &input1_tmp /*filter*/,
                                                         1 /*stride_y*/,
                                                         1 /*stride_x*/,
                                                         1 /*dilation_y*/,
                                                         1 /*dilation_x*/,
                                                         1 /*group*/,
                                                         nullptr /*bias*/,
                                                         m_activation,
                                                         nullptr,
                                                         mode); // do not support PReLU and Leaky RelU
                    int task_size = m_args.size();
                    if (task_size == 1) { // single task
                        forward_args((void *)&m_args[0]);
                    } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
                        ESP_LOGI("MatMul", "two task...");
                        module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
                    } else {
                        ESP_LOGE("MatMul", "Only support task size is 1 or 2, currently task size is %d", task_size);
                    }
                }

            } else if (std::max(origin_input0_shape.size(), origin_input1_shape.size()) == 4) {
                int input0_batch0 = origin_input0_shape.size() == 2 ? 1
                    : origin_input0_shape.size() == 3               ? 1
                                                                    : origin_input0_shape[0];
                int input1_batch0 = origin_input1_shape.size() == 2 ? 1
                    : origin_input1_shape.size() == 3               ? 1
                                                                    : origin_input1_shape[0];
                int input0_batch1 = origin_input0_shape.size() == 2 ? 1
                    : origin_input0_shape.size() == 3               ? origin_input0_shape[0]
                                                                    : origin_input0_shape[1];
                int input1_batch1 = origin_input1_shape.size() == 2 ? 1
                    : origin_input1_shape.size() == 3               ? origin_input1_shape[0]
                                                                    : origin_input1_shape[1];

                int c = origin_input1_shape[origin_input1_shape.size() - 2];
                int n = origin_input1_shape.back();
                int align = input1->get_dtype() == DATA_TYPE_INT8 ? 16 : 8;
                bool is_align = (c * n % align) == 0;
                input0->set_shape({input0_batch0,
                                   input0_batch1,
                                   origin_input0_shape[origin_input0_shape.size() - 2],
                                   origin_input0_shape.back()});
                input1->set_shape({input1_batch0, input1_batch1, c, n});
                int max_batch0 = std::max(input0_batch0, input1_batch0);
                int max_batch1 = std::max(input0_batch1, input1_batch1);

                TensorBase input1_tmp({1, 1, c, n} /*shape*/,
                                      input1_element /*element*/,
                                      input1->exponent /*exponent*/,
                                      input1->dtype /*dtype*/,
                                      is_align ? false : true /*deep*/,
                                      input1->caps /*caps*/);

                for (int i = 0; i < max_batch0; i++) {
                    int input0_i = input0_batch0 == 1 ? 0 : i;
                    int input1_i = input1_batch0 == 1 ? 0 : i;

                    for (int j = 0; j < max_batch1; j++) {
                        int input0_j = input0_batch1 == 1 ? 0 : j;
                        int input1_j = input1_batch1 == 1 ? 0 : j;

                        // input: NHWC
                        TensorBase input0_tmp({1,
                                               1,
                                               origin_input0_shape[origin_input0_shape.size() - 2],
                                               origin_input0_shape.back()} /*shape*/,
                                              static_cast<char *>(input0_element) +
                                                  input0_dtype_bytes *
                                                      input0->get_element_index({input0_i, input0_j, 0, 0}) /*element*/,
                                              input0->exponent /*exponent*/,
                                              input0->dtype /*dtype*/,
                                              false /*deep*/,
                                              input0->caps /*caps*/);

                        // filter: HWIO
                        if (!is_align) {
                            input1_tmp.assign({1, 1, c, n} /*shape*/,
                                              static_cast<char *>(input1_element) +
                                                  input1_dtype_bytes *
                                                      input1->get_element_index({input1_i, input1_j, 0, 0}) /*element*/,
                                              input1->exponent /*exponent*/,
                                              input1->dtype /*dtype*/);
                        } else {
                            input1_tmp.set_element(static_cast<char *>(input1_element) +
                                                   input1_dtype_bytes *
                                                       input1->get_element_index({input1_i, input1_j, 0, 0}));
                        }

                        // output: NHWC
                        TensorBase output_tmp({1,
                                               1,
                                               origin_output_shape[origin_output_shape.size() - 2],
                                               origin_output_shape.back()} /*shape*/,
                                              static_cast<char *>(output_element) +
                                                  output_dtype_bytes *
                                                      output->get_element_index({i, j, 0, 0}) /*element*/,
                                              output->exponent /*exponent*/,
                                              output->dtype /*dtype*/,
                                              false /*deep*/,
                                              output->caps /*caps*/);

                        std::vector<base::ArgsType<T>> m_args =
                            base::get_conv_operation_args<T>(&output_tmp,
                                                             &input0_tmp,
                                                             padding,
                                                             &input1_tmp /*filter*/,
                                                             1 /*stride_y*/,
                                                             1 /*stride_x*/,
                                                             1 /*dilation_y*/,
                                                             1 /*dilation_x*/,
                                                             1 /*group*/,
                                                             nullptr /*bias*/,
                                                             m_activation,
                                                             nullptr,
                                                             mode); // do not support PReLU and Leaky RelU
                        int task_size = m_args.size();
                        if (task_size == 1) { // single task
                            forward_args((void *)&m_args[0]);
                        } else if (task_size == 2) { // multi task, use semaphore to maintain synchronization.
                            ESP_LOGI("MatMul", "two task...");
                            module_forward_dual_core(this, (void *)&m_args[0], (void *)&m_args[1]);
                        } else {
                            ESP_LOGE(
                                "MatMul", "Only support task size is 1 or 2, currently task size is %d", task_size);
                        }
                    }
                }

            } else {
                ESP_LOGE("MatMul",
                         "Impossible matmul, input0 dims: %d, input1 dims: %d",
                         origin_input0_shape.size(),
                         origin_input1_shape.size());
            }
        }

        input0->set_shape(origin_input0_shape);
        input1->set_shape(origin_input1_shape);
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
        DL_LOG_LAYER_LATENCY_END(this->name, "MatMul");
    }

    /**
     * @brief deserialize MatMul module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *matmul_op = nullptr;

        activation_type_t activation_type;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "activation", activation_type);
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);

        // Create module
        if (quant_type == QUANT_TYPE_SYMM_8BIT || quant_type == QUANT_TYPE_SYMM_16BIT) {
            TensorBase *filter = fbs_model->get_operation_parameter(node_name, 1);
            matmul_op = new MatMul(filter, activation_type, node_name.c_str(), quant_type);
        }

        return matmul_op;
    }

    void print()
    {
        ESP_LOGI("MatMul",
                 "filter: %p, "
                 "activation: %s, "
                 "quant_type: %s.",
                 m_filter,
                 activation_type_to_string(m_activation),
                 quant_type_to_string(quant_type));
    }
};
} // namespace module
} // namespace dl
