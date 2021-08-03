################################################################################################################################
Camera with Terminal `[English] <./README.rst>`_
################################################################################################################################

该示例的输入图片来自摄像头，输出结果打印在终端。该示例演示了以下模型接口在实际项目中的使用情况。

+ `HumanFaceDetectMSR01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_msr01.hpp>`_

+ `HumanFaceDetectMNP01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_mnp01.hpp>`_

+ `CatFaceDetectMN03 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/cat_face_detect_mn03.hpp>`_

+ `移动侦测 <https://github.com/espressif/esp-dl/blob/master/include/image/dl_image.hpp#L322>`_


运行示例
************************************************************************************************
1. 参考首页的 `说明文档 <../../>`_ 配置示例。

2. 烧录程序，运行 IDF 监视器获取实时结果：
   
   .. code:: shell
   
       idf.py flash monitor
   
   以选择模型 ``HumanFaceDetectMSR01`` 为例，当人脸凑近摄像头约 50cm 左右，终端打印结果如下所示：
   
   .. figure:: ./img/result_on_terminal.png
       :align: center
   
       ..
       
   打印结果中包括时间消耗和检测的框选坐标。
