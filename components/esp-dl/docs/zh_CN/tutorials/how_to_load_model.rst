使用 ESP-DL 加载模型
====================

:link_to_translation:`en:[English]`

在本教程中，我们将介绍如何加载一个 ESP-DL 的模型。

准备工作
--------

在开始之前，请确保您已经安装了 ESP-IDF 开发环境，并且已经配置好了您的开发板。此外，您需要有一个预训练的模型文件，并且已经使用 `ESP-PPQ <https://github.com/espressif/esp-ppq>`__ 量化完成并导出为 ``.espdl`` 模型格式。

方法 1：从 ``rodata`` 中加载模型
----------------------------------

1. **在** ``CMakeLists.txt`` **中添加模型文件**

   参考文档 `ESP-IDF 构建系统 <https://docs.espressif.com/projects/esp-idf/zh_CN/stable/esp32/api-guides/build-system.html#cmake-embed-data>`__，将 ``.espdl`` 模型文件添加到芯片 flash 的 ``.rodata`` 段。

   .. code:: cmake

      set(embed_files your_model_path/model_name.espdl)
      idf_component_register(...
                              EMBED_FILES ${embed_files})

2. **在程序中加载模型**

   使用以下方法加载模型：

   .. code:: cpp

      // "_binary_model_name_espdl_start" is composed of three parts: the prefix "binary", the filename "model_name_espdl", and the suffix "_start".
      extern const uint8_t espdl_model[] asm("_binary_model_name_espdl_start");

      Model *model = new Model((const char *)espdl_model, fbs::MODEL_LOCATION_IN_FLASH_RODATA);


方法 2：从 ``partition`` 中加载模型
-------------------------------------

1. **在** ``partition.csv`` **中添加模型信息**

   在 ``partition.csv`` 文件中添加模型的 ``offset``、``size`` 等信息。

   ::

      # Name,   Type, SubType, Offset,  Size, Flags
      factory,  app,  factory,  0x010000,  4000K,
      model,   data,  spiffs,        ,  4000K,

2. **在** ``CMakeLists.txt`` **中添加自动加载程序**

   如果选择手动烧写，可以跳过此步骤。

   .. code:: cmake

      set(image_file your_model_path)
      partition_table_get_partition_info(size "--partition-name model" "size")
      if("${size}")
            esptool_py_flash_to_partition(flash "model" "${image_file}")
      else()

3. **在程序中加载模型**

   有两种方法可以加载模型。

   -  使用构造函数加载模型：

      .. code:: cpp

         // method1:
         Model *model = new Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);

   -  首先加载 ``fbs_model``，然后使用 ``fbs_model`` 指针创建模型：

      .. code:: cpp

         // method2:
         fbs::FbsLoader *fbs_loader = new fbs::FbsLoader("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
         fbs::FbsModel *fbs_model = fbs_loader->load();
         Model *model2 = new Model(fbs_model);

通过以上步骤，可以使用 ESP-DL 库成功加载一个预训练的模型。希望本教程对您有所帮助。更多信息请参考 :project_file:`fbs_loader.cpp <esp-dl/fbs_loader/src/fbs_loader.cpp>` 和 :project_file:`fbs_loader.hpp<esp-dl/fbs_loader/include/fbs_loader.hpp>`。
