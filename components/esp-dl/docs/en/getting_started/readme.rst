****************
Getting Started
****************

:link_to_translation:`zh_CN:[中文]`

Hardware Requirements
~~~~~~~~~~~~~~~~~~~~~

.. list::

   - An ESP32-S3 or ESP32-P4 development board. Recommendation: ESP32-S3-EYE or ESP32-P4-Function-EV-Board 
   - PC (Linux)

.. note::
   Some development boards currently have the Type C interface. Make sure you use the proper cable to connect the board!


Software Requirements
---------------------

- **ESP-IDF**

  ESP-DL runs based on ESP-IDF. For detailed instructions on how to get ESP-IDF, please see the `ESP-IDF Programming Guide <https://idf.espressif.com>`_.

  .. note::
     Please use `ESP-IDF <https://github.com/espressif/esp-idf>`_ ``release/v5.3`` or above.

- **ESP-PPQ**

  ESP-PPQ is a quantization tool based on ppq. If you want to quantize your own model, please install esp-ppq using the following command:

  .. code-block:: bash

     pip uninstall ppq
     pip install git+https://github.com/espressif/esp-ppq.git

Model Quantization
------------------

First, please refer to the `ESP-DL Operator Support State <../../../operator_support_state.md>`_ to ensure that the operators in your model are already supported.

ESP-PPQ can directly read ONNX models for quantization. Pytorch and TensorFlow need to be converted to ONNX models first, so make sure your model can be converted to ONNX models.

We provide the following Python script templates. Please select the appropriate template to quantize your models. For more details about quantization, please refer to :doc:`tutorials/how_to_quantize_model <../tutorials/how_to_quantize_model>`.

- `quantize_onnx_model.py <../../../tools/quantization/quantize_onnx_model.py>`_: quantize ONNX models
- `quantize_torch_model.py <../../../tools/quantization/quantize_torch_model.py>`_: quantize Pytorch models
- `quantize_tf_model.py <../../../tools/quantization/quantize_tf_model.py>`_: quantize TensorFlow models

Model Deployment
----------------

ESP-DL provides a series of APIs to quickly load and run models. A typical example is as follows:

.. code-block:: cpp

   #include "dl_model_base.hpp"

   extern const uint8_t espdl_model[] asm("_binary_model_name_espdl_start");
   Model *model = new Model((const char *)espdl_model, fbs::MODEL_LOCATION_IN_FLASH_RODATA);
   model->run(inputs); // inputs is a tensor or a map of tensors

For more details, please refer to :doc:`tutorials/how_to_load_model <../tutorials/how_to_load_model>` and `mobilenet_v2 examples <../../../examples/mobilenet_v2/>`_.

