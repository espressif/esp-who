#include "dl_model_base.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "fbs_loader.hpp"
#include <type_traits>

static const char *TAG = "MOBILENET_V2_EXAMPLE";

// using namespace fbs;
using namespace dl;

template <typename ground_truth_element_t>
void compare_elementwise(const ground_truth_element_t *ground_truth, TensorBase *infer_value)
{
    if (!ground_truth || !infer_value || !infer_value->get_element_ptr()) {
        ESP_LOGE(TAG,
                 "empty data, ground_truth: %p, infer_value: %p, infer_value->get_element_ptr(): %p",
                 ground_truth,
                 infer_value,
                 infer_value->get_element_ptr());
        return;
    }

    ESP_LOGI(TAG, "output size: %d", infer_value->get_size());
    ground_truth_element_t *infer_value_pointer = static_cast<ground_truth_element_t *>(infer_value->get_element_ptr());

    for (int i = 0; i < infer_value->get_size(); i++) {
        if (ground_truth[i] != infer_value_pointer[i]) {
            ESP_LOGE(TAG,
                     "Inconsistent values, ground true: %ld, infer: %ld",
                     static_cast<int32_t>(ground_truth[i]),
                     static_cast<int32_t>(infer_value_pointer[i]));
            std::vector<int> value_position = infer_value->get_axis_index(i);
            ESP_LOGE(TAG, "The position of inconsistent values: %s", dl::shape_to_string(value_position).c_str());
        }
    }
}

void compare_test_outputs(Model *model, std::map<std::string, TensorBase *> infer_outputs)
{
    if (!model) {
        return;
    }

    fbs::FbsModel *fbs_model_instance = model->get_fbs_model();
    fbs_model_instance->load_map();
    for (auto infer_outputs_iter = infer_outputs.begin(); infer_outputs_iter != infer_outputs.end();
         infer_outputs_iter++) {
        std::string infer_output_name = infer_outputs_iter->first;
        const void *ground_truth_data = fbs_model_instance->get_test_output_tensor_raw_data(infer_output_name);
        if (!ground_truth_data) {
            ESP_LOGE(TAG, "The infer output(%s) isn't found in model's ground truth.", infer_output_name.c_str());
            return;
        }
        TensorBase *infer_output = infer_outputs_iter->second;
        ESP_LOGI(TAG,
                 "infer_output, name: %s, shape: %s",
                 infer_outputs_iter->first.c_str(),
                 dl::shape_to_string(infer_output->get_shape()).c_str());

        if (infer_output->dtype == dl::DATA_TYPE_INT8) {
            compare_elementwise(static_cast<const int8_t *>(ground_truth_data), infer_output);
        } else if (infer_output->dtype == dl::DATA_TYPE_INT16) {
            compare_elementwise(static_cast<const int16_t *>(ground_truth_data), infer_output);
        } else if (infer_output->dtype == dl::DATA_TYPE_FLOAT) {
        }
    }

    return;
}

std::map<std::string, TensorBase *> get_graph_test_inputs(Model *model)
{
    std::map<std::string, TensorBase *> test_inputs;

    if (!model) {
        return test_inputs;
    }

    fbs::FbsModel *parser_instance = model->get_fbs_model();
    parser_instance->load_map();
    std::map<std::string, TensorBase *> graph_inputs = model->get_inputs();
    for (auto graph_inputs_iter = graph_inputs.begin(); graph_inputs_iter != graph_inputs.end(); graph_inputs_iter++) {
        std::string input_name = graph_inputs_iter->first;
        TensorBase *input = graph_inputs_iter->second;

        if (input) {
            const void *input_data = parser_instance->get_test_input_tensor_raw_data(input_name);
            if (input_data) {
                TensorBase *test_input =
                    new TensorBase(input->shape, input_data, input->exponent, input->dtype, false, MALLOC_CAP_SPIRAM);
                test_inputs.emplace(input_name, test_input);
            }
        }
    }

    return test_inputs;
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "get into app_main");
    Model *model = new Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
    std::map<std::string, TensorBase *> graph_test_inputs = get_graph_test_inputs(model);
    model->run(graph_test_inputs);

    ::compare_test_outputs(model, model->get_outputs());
    for (auto graph_test_inputs_iter = graph_test_inputs.begin(); graph_test_inputs_iter != graph_test_inputs.end();
         graph_test_inputs_iter++) {
        if (graph_test_inputs_iter->second) {
            delete graph_test_inputs_iter->second;
        }
    }

    graph_test_inputs.clear();
    delete model;
    ESP_LOGI(TAG, "exit app_main");
}
