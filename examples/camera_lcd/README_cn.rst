################################################################################################################################
Camera with LCD `[English] <./README.rst>`_
################################################################################################################################

该示例的输入图片来自摄像头，输出结果显示在 LCD 屏上。该示例演示了以下模型接口在实际项目中的使用情况。

+ `HumanFaceDetectMSR01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_msr01.hpp>`_

+ `HumanFaceDetectMNP01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_mnp01.hpp>`_

+ `CatFaceDetectMN03 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/cat_face_detect_mn03.hpp>`_


支持的开发套件
************************************************************************************************

+ `ESP-S3-EYE <https://www.espressif.com/zh-hans/products/devkits/esp-eye/overview>`_

运行示例
************************************************************************************************
1. 参考首页的 `说明文档 <../../>`_ 配置示例。

   .. attention::
       
       该示例仅支持 RGB565

2. 烧录程序，运行 IDF 监视器：
   
   .. code:: shell
   
       idf.py flash monitor

   您将在 ESP-S3-EYE 的 LCD 屏上看到实时效果，在终端上看到各部分的耗时。






