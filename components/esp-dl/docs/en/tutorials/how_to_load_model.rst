Loading Models with ESP-DL
==========================

:link_to_translation:`zh_CN:[中文]`

In this tutorial, we will guide you through the process of loading an ESP-DL model.

Prerequisites
-------------

Before you begin, ensure that you have the ESP-IDF development environment installed and your development board properly configured. Additionally, you need to have a pre-trained model file that has been quantized using `ESP-PPQ <https://github.com/espressif/esp-ppq>`__ and exported in the ``.espdl`` model format.

Method 1: Load Model from ``rodata``
----------------------------------------

1. **Add Model File in** ``CMakeLists.txt``

   Refer to the documentation `ESP-IDF Build System <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html#cmake-embed-data>`__ to add the ``.espdl`` model file to the ``.rodata`` section of the chip flash.

   .. code:: cmake

      set(embed_files your_model_path/model_name.espdl)
      idf_component_register(...
                              EMBED_FILES ${embed_files})

2. **Load Model in Your Program**

   Use the following method to load the model:

   .. code:: cpp

      // "_binary_model_name_espdl_start" is composed of three parts: the prefix "binary", the filename "model_name_espdl", and the suffix "_start".
      extern const uint8_t espdl_model[] asm("_binary_model_name_espdl_start");

      Model *model = new Model((const char *)espdl_model, fbs::MODEL_LOCATION_IN_FLASH_RODATA);


Method 2: Load Model from ``partition``
-------------------------------------------

1. **Add Model Information in** ``partition.csv``

   Add the model’s ``offset``, ``size``, and other information in the ``partition.csv`` file.

   ::

      # Name,   Type, SubType, Offset,  Size, Flags
      factory,  app,  factory,  0x010000,  4000K,
      model,   data,  spiffs,        ,  4000K,

2. **Add Automatic Loading Program in** ``CMakeLists.txt``

   Skip this step if you choose to manually flash.

   .. code:: cmake

      set(image_file your_model_path)
      partition_table_get_partition_info(size "--partition-name model" "size")
      if("${size}")
            esptool_py_flash_to_partition(flash "model" "${image_file}")
      else()

3. **Load Model in Your Program**

   There are two methods to load the model.

   -  Load the model using the constructor:

      .. code:: cpp

         // method1:
         Model *model = new Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);

   -  First load the ``fbs_model``, then create the model using the ``fbs_model`` pointer:

      .. code:: cpp

         // method2:
         fbs::FbsLoader *fbs_loader = new fbs::FbsLoader("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
         fbs::FbsModel *fbs_model = fbs_loader->load();
         Model *model2 = new Model(fbs_model);

By following the steps above, you can successfully load a pre-trained model using the ESP-DL library. We hope this tutorial is helpful to you! For more information, please refer to the code in :project_file:`fbs_loader.cpp <esp-dl/fbs_loader/src/fbs_loader.cpp>` and :project_file:`fbs_loader.hpp<esp-dl/fbs_loader/include/fbs_loader.hpp>`.
