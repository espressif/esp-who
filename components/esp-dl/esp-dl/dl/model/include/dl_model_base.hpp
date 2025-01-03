#pragma once

#include "dl_constant.hpp"
#include "dl_memory_manager.hpp"
#include "dl_module_base.hpp"
#include "dl_variable.hpp"
#include "esp_log.h"
#include "fbs_loader.hpp"
#include "fbs_model.hpp"
namespace dl {

// currently only support MEMORY_MANAGER_GREEDY
typedef enum { MEMORY_MANAGER_GREEDY = 0, LINEAR_MEMORY_MANAGER = 1 } memory_manager_t;

/**
 * @brief Neural Network Model.
 */
class Model {
private:
    fbs::FbsLoader *fbs_loader = nullptr; /*<! The instance of flatbuffers Loader >*/
    fbs::FbsModel *fbs_model = nullptr;   /*<! The instance of flatbuffers Model >*/
    std::vector<dl::module::Module *>
        execution_plan; /*<! This represents a valid topological sort (dependency ordered) execution plan. >*/
    dl::memory::MemoryManagerBase *memory_manager = nullptr; /*<! The pointer of memory manager >*/
    std::map<std::string, TensorBase *> inputs;              /*  The map of model input's name and TensorBase* */
    std::map<std::string, TensorBase *> outputs;             /*  The map of model output's name and TensorBase* */
    std::string name;                                        /*  The name of model */
    int64_t version;                                         /*  The version of model */
    std::string doc_string;                                  /*  doc string of model*/

public:
    Model() {}

    /**
     * @brief Create the Model object by rodata address or partition label.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location      The model location.
     * @param internal_size  Internal ram size, in bytes
     * @param mm_type        Type of memory manager
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    Model(const char *rodata_address_or_partition_label_or_path,
          fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
          int internal_size = 0,
          memory_manager_t mm_type = MEMORY_MANAGER_GREEDY,
          uint8_t *key = nullptr,
          bool param_copy = true);

    /**
     * @brief Create the Model object by rodata address or partition label.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param model_index   The model index of packed models.
     * @param location      The model location.
     * @param internal_size  Internal ram size, in bytes
     * @param mm_type        Type of memory manager
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    Model(const char *rodata_address_or_partition_label_or_path,
          int model_index,
          fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
          int internal_size = 0,
          memory_manager_t mm_type = MEMORY_MANAGER_GREEDY,
          uint8_t *key = nullptr,
          bool param_copy = true);

    /**
     * @brief Create the Model object by rodata address or partition label.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param model_name   The model name of packed models.
     * @param location      The model location.
     * @param internal_size  Internal ram size, in bytes
     * @param mm_type        Type of memory manager
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    Model(const char *rodata_address_or_partition_label_or_path,
          const char *model_name,
          fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
          int internal_size = 0,
          memory_manager_t mm_type = MEMORY_MANAGER_GREEDY,
          uint8_t *key = nullptr,
          bool param_copy = true);

    /**
     * @brief Create the Model object by fbs_model.
     *
     * @param fbs_model      The fbs model.
     * @param internal_size  Internal ram size, in bytes
     * @param mm_type        Type of memory manager
     */
    Model(fbs::FbsModel *fbs_model, int internal_size = 0, memory_manager_t mm_type = MEMORY_MANAGER_GREEDY);

    /**
     * @brief Destroy the Model object.
     */
    virtual ~Model();

    /**
     * @brief Load model graph and parameters from flash or sdcard.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location      The model location.
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    virtual esp_err_t load(const char *rodata_address_or_partition_label_or_path,
                           fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
                           uint8_t *key = nullptr,
                           bool param_copy = true);

    /**
     * @brief Load model graph and parameters from flash or sdcard.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location      The model location.
     * @param model_index   The model index of packed models.
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    virtual esp_err_t load(const char *rodata_address_or_partition_label_or_path,
                           fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
                           int model_index = 0,
                           uint8_t *key = nullptr,
                           bool param_copy = true);

    /**
     * @brief Load model graph and parameters from flash or sdcard.
     *
     * @param rodata_address_or_partition_label_or_path
     *                                     The address of model data while location is MODEL_LOCATION_IN_FLASH_RODATA.
     *                                     The label of partition while location is MODEL_LOCATION_IN_FLASH_PARTITION.
     *                                     The path of model while location is MODEL_LOCATION_IN_SDCARD.
     * @param location      The model location.
     * @param model_name    The model name of packed models.
     * @param key           The key of encrypted model.
     * @param param_copy    Set to false to avoid copy model parameters from flash to psram.
     *                      Only set this param to false when your psram resource is very tight. This saves psram and
     *                      sacrifices the performance of model inference because the frequency of psram is higher than
     * flash. Only takes effect when MODEL_LOCATION_IN_FLASH_RODATA(CONFIG_SPIRAM_RODATA not set) or
     * MODEL_LOCATION_IN_FLASH_PARTITION.
     */
    virtual esp_err_t load(const char *rodata_address_or_partition_label_or_path,
                           fbs::model_location_type_t location = fbs::MODEL_LOCATION_IN_FLASH_RODATA,
                           const char *model_name = nullptr,
                           uint8_t *key = nullptr,
                           bool param_copy = true);

    /**
     * @brief Load model graph and parameters from Flatbuffers model
     *
     * @param fbs_model          The FlatBuffers model
     */
    virtual esp_err_t load(fbs::FbsModel *fbs_model);

    /**
     * @brief Allocate memory for the model.
     *
     * @param internal_size  Internal ram size, in bytes
     * @param mm_type        Type of memory manager
     * @param preload        Whether to preload the model's parameters to internal ram (not implemented yet)
     */
    virtual void build(size_t internal_size, memory_manager_t mm_type = MEMORY_MANAGER_GREEDY, bool preload = false);

    /**
     * @brief Run the model module by module.
     *
     * @param mode  Runtime mode.
     */
    virtual void run(runtime_mode_t mode = RUNTIME_MODE_SINGLE_CORE);

    /**
     * @brief Run the model module by module.
     *
     * @param input  The model input.
     * @param mode   Runtime mode.
     */
    virtual void run(TensorBase *input, runtime_mode_t mode = RUNTIME_MODE_SINGLE_CORE);

    /**
     * @brief Run the model module by module.
     *
     * @param user_inputs   The model inputs.
     * @param mode          Runtime mode.
     * @param user_outputs  It's for debug to pecify the output of the intermediate layer; Under normal use, there is no
     *                      need to pass a value to this parameter. If no parameter is passed, the default is the
     * graphical output, which can be obtained through Model::get_outputs().
     */
    virtual void run(std::map<std::string, TensorBase *> &user_inputs,
                     runtime_mode_t mode = RUNTIME_MODE_SINGLE_CORE,
                     std::map<std::string, TensorBase *> user_outputs = {});

    /**
     * @brief Get inputs of model
     *
     * @return The map of model input's name and TensorBase*
     */
    virtual std::map<std::string, TensorBase *> &get_inputs();

    /**
     * @brief Get intermediate TensorBase of model
     *
     * @return The intermediate TensorBase*. Note: When using memory manager,
     *         the content of TensorBase's data may be overwritten by the
     *         outputs of other operators.
     */
    virtual TensorBase *get_intermediate(std::string name);

    /**
     * @brief Get outputs of model
     *
     * @return The map of model output's name and TensorBase*
     */
    virtual std::map<std::string, TensorBase *> &get_outputs();

    /**
     * @brief Print the model.
     */
    virtual void print();

    /**
     * @brief Get the fbs model instance.
     *
     * @return fbs::FbsModel *
     */
    virtual fbs::FbsModel *get_fbs_model() { return fbs_model; }
};

} // namespace dl
