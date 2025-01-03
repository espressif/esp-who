创建新模块（算子）
==================

:link_to_translation:`en:[English]`

本教程将指导您在 ``dl::module`` 命名空间中创建一个新模块。``Module`` 类是所有模块的基类，您将扩展这个基类来创建您的自定义模块。

.. note::

    ESP-DL 中的模块接口应与 ONNX 对齐。


理解基类 ``Module``
-------------------

基类提供了几个必须在派生类中重写的虚方法。

- **方法：**

    - :cpp:func:`dl::module::Module::Module`：构造函数，用于初始化模块。
    -  :cpp:func:`dl::module::Module::~Module`：析构函数，用于释放资源。
    -  :cpp:func:`dl::module::Module::get_output_shape`：根据输入形状计算输出形状。
    - :cpp:func:`dl::module::Module::forward`：运行模块，高级接口。
    - :cpp:func:`dl::module::Module::forward_args`：运行模块，低级接口。
    - :cpp:func:`dl::module::Module::deserialize`：从序列化信息创建模块实例。
    - :cpp:func:`dl::module::Module::print`：打印模块信息。

更多信息，请参考 :project_file:`Module Class Reference<esp-dl/dl/module/include/dl_module_base.hpp>`。

创建新模块类
------------

要创建一个新模块，您需要从 ``Module`` 基类派生一个新类并重写必要的方法。

示例：创建 ``MyCustomModule`` 类
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

更多示例，请参考 :project:`esp-dl/dl/module<esp-dl/dl/module/include>`。

.. code:: cpp

    #include "module.h" // 包含定义 Module 类的头文件

    namespace dl {
    namespace module {

    class MyCustomModule : public Module {
    public:
        // 构造函数
        MyCustomModule(const char *name = "MyCustomModule",
                    module_inplace_t inplace = MODULE_NON_INPLACE,
                    quant_type_t quant_type = QUANT_TYPE_NONE)
            : Module(name, inplace, quant_type) {}

        // 析构函数
        virtual ~MyCustomModule() {}

        // 重写 get_output_shape 方法
        std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes) override {
            // 实现根据输入形状计算输出形状的逻辑
            std::vector<std::vector<int>> output_shapes;
            // 示例：假设输出形状与输入形状相同
            output_shapes.push_back(input_shapes[0]);
            return output_shapes;
        }

        // 重写 forward 方法
        void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode = RUNTIME_MODE_AUTO) override {
            // 实现运行模块的逻辑
            // 示例：对张量执行某些操作
            for (auto &tensor : tensors) {
                // 对每个张量执行某些操作
            }
        }

        // 重写 forward_args 方法
        void forward_args(void *args) override {
            // 实现低级接口的逻辑
            // 示例：根据参数执行某些操作
        }

        // 从序列化信息反序列化模块实例
        static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name){
            // 实现反序列化模块实例的逻辑
            // 接口应与 ONNX 对齐
        }

        // 重写 print 方法
        void print() override {
            // 打印模块信息
            ESP_LOGI("MyCustomModule", "Module Name: %s, Quant type: %d", name.c_str(), quant_type);
        }
    };

    } // namespace module
    } // namespace dl

注册 ``MyCustomModule`` 类
~~~~~~~~~~~~~~~~~~~~~~~~~~

当您实现了 ``MyCustomModule`` 类后，请在 :project_file:`dl_module_creator <esp-dl/dl/module/include/dl_module_creator.hpp>` 中注册您的模块，使其全局可用。

.. code:: cpp

    void register_dl_modules()
    {
        if (creators.empty()) {
            ...
            this->register_module("MyCustomModule", MyCustomModule::deserialize);
        }
    }
