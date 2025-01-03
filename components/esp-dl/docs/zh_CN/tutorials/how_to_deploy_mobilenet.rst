使用 ESP-DL 部署模型
====================

:link_to_translation:`en:[English]`

在本教程中，我们将介绍如何量化模型，如何将 ESP-DL 前向推理结果与电脑端 ESP-PPQ 前向推理结果做比较。其重在介绍如何量化模型，如何部署模型，不涉及模型输入数据的获取和处理，模型输入为随机数。


准备工作
--------

在开始之前，请确保您已经安装了 ESP-IDF 开发环境，并且已经配置好了您的开发板。

除此之外，您还需要安装量化工具 `ESP-PPQ <https://github.com/espressif/esp-ppq>`__。该工具基于优秀的开源量化工具 `PPQ <https://github.com/OpenPPL/ppq>`__，添加适合乐鑫芯片平台的客制化配置形成。

.. code:: bash

   pip uninstall ppq
   pip install git+https://github.com/espressif/esp-ppq.git


模型量化
--------

MobileNet_v2 模型量化请参考 :doc:`how_to_quantize_model`。


模型部署及推理精度测试
----------------------

示例工程见 :project:`examples/mobilenet_v2`，其目录结构如下：

.. code:: bash

   $ tree examples/mobilenet_v2
   examples/mobilenet_v2
   ├── CMakeLists.txt
   ├── main
   │   ├── app_main.cpp
   │   ├── CMakeLists.txt
   │   └── Kconfig.projbuild
   ├── models
   │   ├── mobilenet_v2.espdl
   │   ├── mobilenet_v2.info
   │   ├── mobilenet_v2.json
   │   └── mobilenet_v2.onnx
   ├── pack_model.py
   ├── partitions_esp32p4.csv
   ├── sdkconfig.defaults
   └── sdkconfig.defaults.esp32p4

   2 directories, 12 files

主要文件介绍如下：

- ``main/app_main.cpp`` 展示了如何调用 ESP-DL 接口加载、运行模型。
- ``models`` 目录存放模型相关文件，其中只有 ``mobilenet_v2.espdl`` 文件是必须的，将会被烧录到 flash 分区中。
- ``pack_model.py`` 为模型打包脚本，由 ``main/CMakeLists.txt`` 调用执行。
- ``partitions_esp32p4.csv`` 是分区表，在该工程中，模型文件 ``models/mobilenet_v2.espdl`` 将会被烧录到其中的 ``model`` 分区。
- ``sdkconfig.defaults.esp32p4`` 是项目配置，其中 ``CONFIG_MODEL_FILE_PATH`` 配置了模型文件路径，是基于该项目的相对路径。

模型加载运行
~~~~~~~~~~~~

ESP-DL 支持自动构图及内存规划，目前支持的算子见 :project:`esp-dl/dl/module/include`。

对于模型的加载运行，只需要参照下方示例，简单调用几个接口即可。该示例采用构造函数，以系统分区的形式加载模型。更多加载方式请参考 :doc:`how_to_load_model`。

.. code:: cpp

   Model *model = new Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
   ......
   model->run(graph_test_inputs);

模型输入 ``graph_test_inputs``，在该示例中，通过 ``get_graph_test_inputs`` 函数获得。

如下所示，该函数实现主要是构建 ``TensorBase`` 对象，传参 ``input_data`` 为模型输入数据 buffer 的首地址，buffer 中的数据需要是已经量化后的数据。

由于该示例展示的是如何测试 ESP-DL 推理精度，所以这里 ``input_data`` 获取的是已经被 ESP-PPQ 打包进 ``mobilenet_v2.espdl`` 文件中的测试输入值。 **input_data 需要是首地址 16 字节对齐的内存块，可通过 IDF 接口** ``heap_caps_aligned_alloc`` **分配。**

.. code:: cpp

   const void *input_data = parser_instance->get_test_input_tensor_raw_data(input_name);
   if (input_data) {
         TensorBase *test_input =
            new TensorBase(input->shape, input_data, input->exponent, input->dtype, false, MALLOC_CAP_SPIRAM);
         test_inputs.emplace(input_name, test_input);
   }

.. note::

    对于输入数据的量化处理，ESP-DL P4 采用的 round 策略为 "Rounding half to even"，可参考 :project_file:`bool TensorBase::assign(TensorBase *tensor) <esp-dl/dl/tensor/src/dl_tensor_base.cpp>` 中相关实现。量化所需的 exponent 等信息，可在 ``*.info`` 相关模型文件中查找。

推理结果获取及测试
~~~~~~~~~~~~~~~~~~

在 ``model->run(graph_test_inputs)`` 运行完之后，我们就可以通过 ``model->get_outputs()`` 获取 ESP-DL 的推理结果了，返回的是 std::map 对象。之后，就可以参考 ``compare_test_outputs`` 函数实现，与模型文件中的 ESP-PPQ 推理结果做比较。 如果需要在 ESP-DL 中获取模型推理的中间结果，则需额外构建中间层对应 ``TensorBase`` 对象，与其名字组成 ``std::map`` 对象传给 ``user_outputs`` 入参。``TensorBase`` 对象的构造参照前面 ``inputs TensorBase`` 对象的构造。

.. code:: cpp

   void Model::run(std::map<std::string, TensorBase *> &user_inputs,
                  runtime_mode_t mode,
                  std::map<std::string, TensorBase *> user_outputs);
