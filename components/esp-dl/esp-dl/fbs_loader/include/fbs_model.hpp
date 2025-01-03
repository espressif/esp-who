#pragma once

#include "dl_tensor_base.hpp"
#include "dl_tool.hpp"
#include "esp_log.h"
#include <limits>
#include <map>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace fbs {

/**
 * @brief Flatbuffer model object.
 */
class FbsModel {
public:
    /**
     * @brief Construct a new FbsModel object.
     *
     * @param name          The label of partition while location is MODEL_LOCATION_IN_FLASH.
     *                      The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location      The model location.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    FbsModel(const void *data, bool auto_free = false, bool param_copy = true);

    /**
     * @brief Destroy the FbsModel object.
     */
    ~FbsModel();

    /**
     * @brief Print the model information.
     */
    void print();

    /**
     * @brief Return vector of node name in the order of execution.
     *
     * @return topological sort of node name.
     */
    std::vector<std::string> topological_sort();

    /**
     * @brief Get the attribute of node.
     *
     * @param node_name         The name of operation.
     * @param attribute_name    The name of attribute.
     * @param ret_value         The attribute value.
     *
     * @return esp_err_t        Return ESP_OK if get successfully. Otherwise return ESP_FAIL.
     */
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, int &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, float &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, std::string &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, std::vector<int> &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, std::vector<float> &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, dl::quant_type_t &ret_value);
    esp_err_t get_operation_attribute(std::string node_name,
                                      std::string attribute_name,
                                      dl::activation_type_t &ret_value);
    esp_err_t get_operation_attribute(std::string node_name, std::string attribute_name, dl::resize_mode_t &ret_value);

    /**
     * @brief Get operation output shape
     *
     * @param node_name         The name of operation.
     * @param index             The index of outputs
     * @param ret_value         Return shape value.
     *
     * @return esp_err_t        Return ESP_OK if get successfully. Otherwise return ESP_FAIL.
     */
    esp_err_t get_operation_output_shape(std::string node_name, int index, std::vector<int> &ret_value);

    /**
     * @brief Get the attribute of node.
     *
     * @param node_name         The name of operation.
     * @param inputs            The vector of operation inputs.
     * @param outputs           The vector of operation outputs.
     *
     * @return esp_err_t        Return ESP_OK if get successfully. Otherwise return ESP_FAIL.
     */
    esp_err_t get_operation_inputs_and_outputs(std::string node_name,
                                               std::vector<std::string> &inputs,
                                               std::vector<std::string> &outputs);

    /**
     * @brief Get operation type, "Conv", "Linear" etc
     *
     * @param node_name  The name of operation
     *
     * @return The type of operation.
     */
    std::string get_operation_type(std::string node_name);

    /**
     * @brief Return if the variable is a parameter
     *
     * @param node_name  The name of operation
     * @param index      The index of the variable
     * @param caps       Bitwise OR of MALLOC_CAP_* flags indicating the type of memory to be returned
     *
     * @return TensorBase
     */
    dl::TensorBase *get_operation_parameter(std::string node_name, int index = 1, uint32_t caps = MALLOC_CAP_SPIRAM);

    /**
     * @brief Get LUT(Look Up Table) if the operation has LUT
     *
     * @param node_name   The name of operation
     * @param caps       Bitwise OR of MALLOC_CAP_* flags indicating the type of memory to be returned
     * @param attribute_name The name of LUT attribute
     */
    dl::TensorBase *get_operation_lut(std::string node_name,
                                      uint32_t caps = MALLOC_CAP_SPIRAM,
                                      std::string attribute_name = "lut");

    /**
     * @brief return true if the variable is a parameter
     *
     * @param name Variable name
     *
     * @return true if the variable is a parameter else false
     */
    bool is_parameter(std::string name);

    /**
     * @brief Get the raw data of FlatBuffers::Dl::Tensor.
     *
     * @param tensor_name   The name of Tensor.
     *
     * @return uint8_t *    The pointer of raw data.
     */
    const void *get_tensor_raw_data(std::string tensor_name);

    /**
     * @brief Get the element type of tensor tensor.
     *
     * @param tensor_name    The tensor name.
     *
     * @return FlatBuffers::Dl::TensorDataType
     */
    dl::dtype_t get_tensor_dtype(std::string tensor_name);

    /**
     * @brief Get the shape of tensor.
     *
     * @param tensor_name       The name of tensor.
     *
     * @return std::vector<int>  The shape of tensor.
     */
    std::vector<int> get_tensor_shape(std::string tensor_name);

    /**
     * @brief Get the exponents of tensor.
     *
     * @warning When quantization is PER_CHANNEL, the size of exponents is same as out_channels.
     *          When quantization is PER_TENSOR, the size of exponents is 1.
     *
     * @param tensor_name       The name of tensor.
     *
     * @return  The exponents of tensor.
     */
    std::vector<int> get_tensor_exponents(std::string tensor_name);

    /**
     * @brief Get the element type of value_info.
     *
     * @param var_name    The value_info name.
     *
     * @return dl::dtype_t
     */
    dl::dtype_t get_value_info_dtype(std::string var_name);

    /**
     * @brief Get the shape of value_info.
     *
     * @param var_name      The value_info name.
     *
     * @return the shape of value_info.
     */
    std::vector<int> get_value_info_shape(std::string var_name);

    /**
     * @brief Get the exponent of value_info. Only support PER_TENSOR quantization.
     *
     * @param var_name      The value_info name.
     *
     * @return the exponent of value_info
     */
    int get_value_info_exponent(std::string var_name);

    /**
     * @brief Get the raw data of test input tensor.
     *
     * @param tensor_name   The name of test input tensor.
     *
     * @return uint8_t *    The pointer of raw data.
     */
    const void *get_test_input_tensor_raw_data(std::string tensor_name);

    /**
     * @brief Get the raw data of test output tensor.
     *
     * @param tensor_name   The name of test output tensor.
     *
     * @return uint8_t *    The pointer of raw data.
     */
    const void *get_test_output_tensor_raw_data(std::string tensor_name);

    /**
     * @brief Get the test input tensor.
     *
     * @param tensor_name   The name of test input tensor.
     * @param caps       Bitwise OR of MALLOC_CAP_* flags indicating the type of memory to be returned
     * @return  The pointer of tensor.
     */
    dl::TensorBase *get_test_input_tensor(std::string tensor_name, uint32_t caps = MALLOC_CAP_SPIRAM);

    /**
     * @brief Get the test output tensor.
     *
     * @param tensor_name   The name of test output tensor.
     * @param caps       Bitwise OR of MALLOC_CAP_* flags indicating the type of memory to be returned
     * @return The pointer of tensor.
     */
    dl::TensorBase *get_test_output_tensor(std::string tensor_name, uint32_t caps = MALLOC_CAP_SPIRAM);

    /**
     * @brief Get the graph inputs.
     *
     * @return the name of inputs
     */
    std::vector<std::string> get_graph_inputs();

    /**
     * @brief Get the graph outputs.
     *
     * @return the name of ounputs
     */
    std::vector<std::string> get_graph_outputs();

    /**
     * @brief Clear all map
     */
    void clear_map();

    /**
     * @brief Load all map
     */
    void load_map();

    /**
     * @brief Get model name
     */
    std::string get_model_name();

    /**
     * @brief Get model version
     */
    int64_t get_model_version();

    /**
     * @brief Get model doc string
     */
    std::string get_model_doc_string();

    bool m_param_copy;

private:
    bool m_auto_free;
    const uint8_t *m_data;
    const void *m_model;
    std::map<std::string, const void *> m_name_to_node_map;
    std::map<std::string, const void *> m_name_to_initial_tensor_map;
    std::map<std::string, const void *> m_name_to_value_info_map;
    std::unordered_map<std::string, const void *> m_name_to_test_inputs_value_map;
    std::unordered_map<std::string, const void *> m_name_to_test_outputs_value_map;
};
} // namespace fbs
