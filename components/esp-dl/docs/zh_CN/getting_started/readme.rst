****************
入门指南
****************

:link_to_translation:`en:[English]`

硬件要求
~~~~~~~~~~~~~~~~~~~~~

.. list::

   - 一块 ESP32-S3 或 ESP32-P4 开发板。推荐使用：ESP32-S3-EYE 或 ESP32-P4-Function-EV-Board
   - 一台 PC（Linux 系统）

.. note::
   部分开发板目前采用 Type C 接口。请确保使用正确的线缆连接开发板！


软件要求
---------------------

- **ESP-IDF**

  ESP-DL 基于 ESP-IDF 运行。有关如何获取 ESP-IDF 的详细说明，请参阅 `ESP-IDF 编程指南 <https://idf.espressif.com>`_。

  .. note::
     请使用 `ESP-IDF <https://github.com/espressif/esp-idf>`_ 的 ``release/v5.3`` 或更高版本。

- **ESP-PPQ**

  ESP-PPQ 是基于 ppq 的量化工具。如果您想量化自己的模型，请使用以下命令安装 esp-ppq：

  .. code-block:: bash

     pip uninstall ppq
     pip install git+https://github.com/espressif/esp-ppq.git

模型量化
------------------

首先，请参考 `ESP-DL 算子支持状态 <../../../operator_support_state.md>`_，确保您的模型中的算子已经得到支持。

ESP-PPQ 可以直接读取 ONNX 模型进行量化。Pytorch 和 TensorFlow 需要先转换为 ONNX 模型，因此请确保您的模型可以转换为 ONNX 模型。

我们提供了以下 Python 脚本模板。请选择合适的模板来量化您的模型。有关量化的更多详细信息，请参阅 :doc:`教程/如何量化模型 <../tutorials/how_to_quantize_model>`。

- `quantize_onnx_model.py <../../../tools/quantization/quantize_onnx_model.py>`_: 量化 ONNX 模型
- `quantize_torch_model.py <../../../tools/quantization/quantize_torch_model.py>`_: 量化 Pytorch 模型
- `quantize_tf_model.py <../../../tools/quantization/quantize_tf_model.py>`_: 量化 TensorFlow 模型

模型部署
----------------

ESP-DL 提供了一系列 API 来快速加载和运行模型。一个典型的示例如下：

.. code-block:: cpp

   #include "dl_model_base.hpp"

   extern const uint8_t espdl_model[] asm("_binary_model_name_espdl_start");  // 模型二进制文件, 存放在rodata段
   Model *model = new Model((const char *)espdl_model, fbs::MODEL_LOCATION_IN_FLASH_RODATA);  // 新建一个模型对象
   model->run(inputs); // inputs 是一个张量或张量映射

更多详细信息，请参阅 :doc:`教程/如何加载模型 <../tutorials/how_to_load_model>` 和 `mobilenet_v2 示例 <../../../examples/mobilenet_v2/>`_。