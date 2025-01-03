#include <stdint.h>

#include "dl_memory_manager_greedy.hpp"
#include "dl_model_base.hpp"
#include "dl_module_creator.hpp"
#include "fbs_model.hpp"

static const char *TAG = "dl::Model";

namespace dl {

Model::Model(const char *name,
             fbs::model_location_type_t location,
             int internal_size,
             memory_manager_t mm_type,
             uint8_t *key,
             bool param_copy)
{
    if (this->load(name, location, key, param_copy) == ESP_OK) {
        this->build(internal_size, mm_type);
    }
}

Model::Model(const char *name,
             int model_index,
             fbs::model_location_type_t location,
             int internal_size,
             memory_manager_t mm_type,
             uint8_t *key,
             bool param_copy)
{
    if (this->load(name, location, model_index, key, param_copy) == ESP_OK) {
        this->build(internal_size, mm_type);
    }
}

Model::Model(const char *name,
             const char *model_name,
             fbs::model_location_type_t location,
             int internal_size,
             memory_manager_t mm_type,
             uint8_t *key,
             bool param_copy)
{
    if (this->load(name, location, model_name, key, param_copy) == ESP_OK) {
        this->build(internal_size, mm_type);
    }
}

Model::Model(fbs::FbsModel *fbs_model, int internal_size, memory_manager_t mm_type)
{
    if (this->load(fbs_model) == ESP_OK) {
        this->build(internal_size, mm_type);
    }
}

Model::~Model()
{
    // If fbs_loader is NULL, this means fbs_model is created outside this class. So don't delete it.
    if (fbs_loader) {
        delete fbs_loader;

        if (fbs_model) {
            delete fbs_model;
        }
    }

    if (memory_manager) {
        delete memory_manager;
    }
    if (!execution_plan.empty()) {
        for (int i = 0; i < execution_plan.size(); i++) {
            delete execution_plan[i];
        }
    }
}

esp_err_t Model::load(const char *name, fbs::model_location_type_t location, uint8_t *key, bool param_copy)
{
    fbs_loader = new fbs::FbsLoader(name, location);
    return this->load(fbs_loader->load(key, param_copy));
}

esp_err_t Model::load(
    const char *name, fbs::model_location_type_t location, int model_index, uint8_t *key, bool param_copy)
{
    fbs_loader = new fbs::FbsLoader(name, location);
    return this->load(fbs_loader->load(model_index, key, param_copy));
}

esp_err_t Model::load(
    const char *name, fbs::model_location_type_t location, const char *model_name, uint8_t *key, bool param_copy)
{
    fbs_loader = new fbs::FbsLoader(name, location);
    return this->load(fbs_loader->load(model_name, key, param_copy));
}

esp_err_t Model::load(fbs::FbsModel *fbs_model)
{
    esp_err_t ret = ESP_OK;
    if (!fbs_model) {
        ESP_LOGE(TAG, "Fail to load model");
        ret = ESP_FAIL;
        return ret;
    }
    this->fbs_model = fbs_model; // fbs_model is created by fbs_loader, so we don't need to delete it.
    fbs_model->load_map();
    this->name = fbs_model->get_model_name();
    this->version = fbs_model->get_model_version();
    this->doc_string = fbs_model->get_model_doc_string();
    if (this->doc_string.empty()) {
        ESP_LOGI(TAG, "model:%s, version:%lld\n", this->name.c_str(), this->version);
    } else {
        ESP_LOGI(TAG,
                 "model:%s, version:%lld, description:%s\n",
                 this->name.c_str(),
                 this->version,
                 this->doc_string.c_str());
    }

    // Construct the execution plan.
    execution_plan.clear();
    dl::module::ModuleCreator *module_creator = dl::module::ModuleCreator::get_instance();

    std::vector<std::string> sorted_nodes = fbs_model->topological_sort();
    for (int i = 0; i < sorted_nodes.size(); i++) {
        std::string node_name = sorted_nodes[i];
        std::string op_type = fbs_model->get_operation_type(node_name);
        ESP_LOGI(TAG, "%s: %s", node_name.c_str(), op_type.c_str());
        if (op_type.empty()) {
            ESP_LOGE(TAG, "Can not find the operation %s", node_name.c_str());
            ret = ESP_FAIL;
            break;
        }
        dl::module::Module *module = module_creator->create(fbs_model, op_type, node_name);
        if (!module) {
            ESP_LOGE(TAG, "Do not support %s, please implement and register it first.", op_type.c_str());
            ret = ESP_FAIL;
            break;
        }
        execution_plan.push_back(module);
    }

    this->memory_manager = nullptr;
    return ret;
}

void Model::build(size_t internal_size, memory_manager_t mm_type, bool preload)
{
    int max_available_internal_size = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) * 0.8;
    if (internal_size > max_available_internal_size) {
        ESP_LOGW(TAG, "The maximum available internal memory is %d", max_available_internal_size);
        internal_size = max_available_internal_size;
    }

    // If memory manager has been created, delete it and reset all modules
    this->fbs_model->load_map();
    if (this->memory_manager) {
        delete this->memory_manager;
        for (int i = 0; i < execution_plan.size(); i++) {
            dl::module::Module *module = execution_plan[i];
            if (module) {
                module->reset();
            }
        }
    }

    if (mm_type == MEMORY_MANAGER_GREEDY) {
        this->memory_manager = new dl::memory::MemoryManagerGreedy(internal_size);
    }
    this->memory_manager->alloc(this->fbs_model, this->execution_plan);

    // get the TensorBase* of inputs and outputs
    std::vector<std::string> inputs_tmp = fbs_model->get_graph_inputs();
    std::vector<std::string> outputs_tmp = fbs_model->get_graph_outputs();
    this->inputs.clear();
    this->outputs.clear();
    for (int i = 0; i < inputs_tmp.size(); i++) {
        TensorBase *input_tensor = this->get_intermediate(inputs_tmp[i]);
        this->inputs.emplace(inputs_tmp[i], input_tensor);
    }
    for (int i = 0; i < outputs_tmp.size(); i++) {
        TensorBase *output_tensor = this->get_intermediate(outputs_tmp[i]);
        this->outputs.emplace(outputs_tmp[i], output_tensor);
    }

    this->fbs_model->clear_map();
}

void Model::run(runtime_mode_t mode)
{
    // execute each module.
    for (int i = 0; i < execution_plan.size(); i++) {
        dl::module::Module *module = execution_plan[i];
        if (module) {
            module->forward(this->memory_manager->tensors, mode);
        } else {
            break;
        }
    }
}

void Model::run(TensorBase *input, runtime_mode_t mode)
{
    if (this->inputs.size() != 1) {
        ESP_LOGW(TAG, "The inputs of model is not jsut one! This API will assign data to first input");
    }

    TensorBase *model_input = this->inputs.begin()->second;
    if (!model_input->assign(input)) {
        ESP_LOGE(TAG, "Assign input failed");
        return;
    }

    // execute each module.
    for (int i = 0; i < execution_plan.size(); i++) {
        dl::module::Module *module = execution_plan[i];
        if (module) {
            // ESP_LOGI(TAG, "module: %d\n", i);
            module->forward(this->memory_manager->tensors, mode);
        } else {
            break;
        }
    }
}

void Model::run(std::map<std::string, TensorBase *> &user_inputs,
                runtime_mode_t mode,
                std::map<std::string, TensorBase *> user_outputs)
{
    if (user_inputs.size() != this->inputs.size()) {
        ESP_LOGE(TAG,
                 "The size of user_inputs(%d) don't equal with the size of model inputs(%d).",
                 user_inputs.size(),
                 this->inputs.size());
        return;
    }

    for (auto user_inputs_iter = user_inputs.begin(); user_inputs_iter != user_inputs.end(); user_inputs_iter++) {
        std::string user_input_name = user_inputs_iter->first;
        TensorBase *user_input_tensor = user_inputs_iter->second;
        auto graph_input_iter = this->inputs.find(user_input_name);
        if (graph_input_iter == this->inputs.end()) {
            ESP_LOGE(TAG, "The input name(%s) isn't graph input.", user_input_name.c_str());
            return;
        }
        TensorBase *graph_input_tensor = graph_input_iter->second;
        if (!graph_input_tensor->assign(user_input_tensor)) {
            ESP_LOGE(TAG, "Assign input failed");
            return;
        }
    }

    // execute each module.
    for (int i = 0; i < execution_plan.size(); i++) {
        dl::module::Module *module = execution_plan[i];
        if (module) {
            module->forward(this->memory_manager->tensors, mode);
            // get the intermediate tensor for debug.
            if (!user_outputs.empty()) {
                for (auto user_outputs_iter = user_outputs.begin(); user_outputs_iter != user_outputs.end();
                     user_outputs_iter++) {
                    int user_tensor_index =
                        this->memory_manager->get_tensor_index(const_cast<std::string &>(user_outputs_iter->first));
                    if (user_tensor_index >= 0) {
                        std::vector<int> outputs_index = module->get_outputs_index();
                        for (int i = 0; i < outputs_index.size(); i++) {
                            if (user_tensor_index == outputs_index[i]) {
                                user_outputs_iter->second->assign(this->memory_manager->tensors[user_tensor_index]);
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            break;
        }
    }
    return;
}

std::map<std::string, TensorBase *> &Model::get_inputs()
{
    return this->inputs;
}

TensorBase *Model::get_intermediate(std::string name)
{
    if (name.empty()) {
        ESP_LOGE(TAG, "Invalid name.");
        return nullptr;
    }
    return this->memory_manager->get_tensor(name);
}

std::map<std::string, TensorBase *> &Model::get_outputs()
{
    return this->outputs;
}

void Model::print()
{
    if (!execution_plan.empty()) {
        for (int i = 0; i < execution_plan.size(); i++) {
            if (execution_plan[i]) {
                ESP_LOGI(TAG, "------------------------------- %d -------------------------------", i);
                if (execution_plan[i]) {
                    execution_plan[i]->print();
                } else {
                    break;
                }
            }
        }
        ESP_LOGI(TAG, "-------------------------------------------------------------\n");
    }
}

} // namespace dl
