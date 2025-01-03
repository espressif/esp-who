#pragma once
#include "dl_module_add.hpp"
#include "dl_module_average_pool.hpp"
#include "dl_module_clip.hpp"
#include "dl_module_concat.hpp"
#include "dl_module_conv.hpp"
#include "dl_module_div.hpp"
#include "dl_module_exp.hpp"
#include "dl_module_flatten.hpp"
#include "dl_module_gather.hpp"
#include "dl_module_gemm.hpp"
#include "dl_module_global_average_pool.hpp"
#include "dl_module_hard_sigmoid.hpp"
#include "dl_module_hard_swish.hpp"
#include "dl_module_leaky_relu.hpp"
#include "dl_module_log.hpp"
#include "dl_module_lut.hpp"
#include "dl_module_matmul.hpp"
#include "dl_module_max_pool.hpp"
#include "dl_module_mul.hpp"
#include "dl_module_pad.hpp"
#include "dl_module_prelu.hpp"
#include "dl_module_relu.hpp"
#include "dl_module_requantize_linear.hpp"
#include "dl_module_reshape.hpp"
#include "dl_module_resize.hpp"
#include "dl_module_sigmoid.hpp"
#include "dl_module_slice.hpp"
#include "dl_module_softmax.hpp"
#include "dl_module_split.hpp"
#include "dl_module_sqrt.hpp"
#include "dl_module_squeeze.hpp"
#include "dl_module_sub.hpp"
#include "dl_module_tanh.hpp"
#include "dl_module_transpose.hpp"
#include "dl_module_unsqueeze.hpp"
#include "fbs_loader.hpp"
#include <functional>
#include <iostream>
#include <malloc.h>
#include <map>
namespace dl {
namespace module {

class ModuleCreator {
public:
    using Creator = std::function<Module *(fbs::FbsModel *, std::string)>; ///< Module creator function type

    /**
     * @brief Get instance of ModuleCreator by this function. It is only safe method to get instance of ModuleCreator
     * becase ModuleCreator is a singleton class.
     *
     * @return ModuleCreator instance pointer
     */
    static ModuleCreator *get_instance()
    {
        // This is thread safe for C++11, please refer to `Meyers' implementation of the Singleton pattern`
        static ModuleCreator instance;
        return &instance;
    }
    /**
     * @brief Register a module creator to the module creator map
     *        This function allows for the dynamic registration of new module types and their corresponding creator
     * functions at runtime. By associating the module type name with the creator function, the system can flexibly
     * create instances of various modules.
     *
     * @param op_type The module type name, used as the key in the map
     * @param creator The module creator function, used to create modules of a specific type
     */
    void register_module(const std::string &op_type, Creator creator) { ModuleCreator::creators[op_type] = creator; }

    /**
     * @brief Create module instance pointer
     *
     * @param fbs_model  Flatbuffer model pointer
     * @param op_type    Module/Operator type
     * @param name       Module name
     *
     * @return Module instance pointer
     */
    Module *create(fbs::FbsModel *fbs_model, const std::string &op_type, const std::string name)
    {
        this->register_dl_modules();

        if (creators.find(op_type) != creators.end()) {
            return creators[op_type](fbs_model, name);
        }
        return nullptr;
    }

    /**
     * @brief Pre-register the already implemented modules
     */
    void register_dl_modules()
    {
        if (creators.empty()) {
            this->register_module("Conv", Conv2D::deserialize);
            this->register_module("Add", Add::deserialize);
            this->register_module("Sub", Sub::deserialize);
            this->register_module("Mul", Mul::deserialize);
            this->register_module("Div", Div::deserialize);
            this->register_module("Resize", Resize2D::deserialize);
            this->register_module("GlobalAveragePool", GlobalAveragePool2D::deserialize);
            this->register_module("AveragePool", AveragePool2D::deserialize);
            this->register_module("Concat", Concat::deserialize);
            this->register_module("Sigmoid", Sigmoid::deserialize);
            this->register_module("Tanh", Tanh::deserialize);
            this->register_module("Relu", Relu::deserialize);
            this->register_module("LeakyRelu", LeakyRelu::deserialize);
            this->register_module("HardSigmoid", HardSigmoid::deserialize);
            this->register_module("HardSwish", HardSwish::deserialize);
            this->register_module("Gelu", LUT::deserialize);
            this->register_module("Elu", LUT::deserialize);
            this->register_module("LUT", LUT::deserialize);
            this->register_module("Gemm", Gemm::deserialize);
            this->register_module("QuantizeLinear", RequantizeLinear::deserialize);
            this->register_module("DequantizeLinear", RequantizeLinear::deserialize);
            this->register_module("RequantizeLinear", RequantizeLinear::deserialize);
            this->register_module("PRelu", PRelu::deserialize);
            this->register_module("Clip", Clip::deserialize);
            this->register_module("Flatten", Flatten::deserialize);
            this->register_module("Reshape", Reshape::deserialize);
            this->register_module("Transpose", Transpose::deserialize);
            this->register_module("Exp", Exp::deserialize);
            this->register_module("Log", Log::deserialize);
            this->register_module("Sqrt", Sqrt::deserialize);
            this->register_module("Squeeze", Squeeze::deserialize);
            this->register_module("Unsqueeze", Unsqueeze::deserialize);
            this->register_module("Softmax", Softmax::deserialize);
            this->register_module("MaxPool", MaxPool2D::deserialize);
            this->register_module("Slice", Slice::deserialize);
            this->register_module("Pad", Pad::deserialize);
            this->register_module("MatMul", MatMul::deserialize);
            this->register_module("Split", Split::deserialize);
            this->register_module("Gather", Gather::deserialize);
        }
    }

    /**
     * @brief Print all modules has been registered
     */
    void print()
    {
        if (!creators.empty()) {
            for (auto it = creators.begin(); it != creators.end(); ++it) {
                printf("%s", (*it).first.c_str());
            }
        } else {
            printf("Create empty module\n");
        }
    }

    /**
     * @brief Clear all modules has been registered
     */
    void clear()
    {
        if (!creators.empty()) {
            creators.clear();
            malloc_trim(0);
        }
    }

private:
    ModuleCreator() {}
    ~ModuleCreator() {}
    ModuleCreator(const ModuleCreator &) = delete;
    ModuleCreator &operator=(const ModuleCreator &) = delete;
    std::map<std::string, Creator> creators;
};

} // namespace module
} // namespace dl
