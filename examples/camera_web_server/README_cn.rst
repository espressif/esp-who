################################################################################################################################
Camera with Web Server `[English] <./README.rst>`_
################################################################################################################################

该示例的输入图片来自摄像头，输出结果网页端。该示例演示了以下模型接口在实际项目中的使用情况。

+ `HumanFaceDetectMSR01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_msr01.hpp>`_

+ `HumanFaceDetectMNP01 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/human_face_detect_mnp01.hpp>`_

+ `CatFaceDetectMN03 <https://github.com/espressif/esp-dl/blob/master/include/model_zoo/cat_face_detect_mn03.hpp>`_



运行示例
************************************************************************************************
1. 参考首页的 `说明文档 <../../>`_ 配置示例。

2. 该示例具有属于自己的示例配置，可配置 WiFi 属性，如下图：
   
   .. figure:: ./img/example-config.png
       :align: center
   
       ..
   
3. 烧录程序，运行 IDF 监视器：
   
   .. code:: shell
   
       idf.py flash monitor

4. 连接 WiFi，以上图配置为例，WiFi 名为 ``myWiFi``，WiFi 密码为 ``00000000``。

5. 打开浏览器，并登录 IP 地址，以上图配置为例，IP 地址为为 ``192.168.4.1``。

6. 您可以
   
   + ``Get Still`` 拍摄一张照片
   
   + ``Start Stream`` 连续获取图片，此时，
     
     + 打开 ``Detection`` 可使能检测功能
     
     + 打开 ``Recognition`` 可使能识别功能（当然您在配置时使能了这个选项）
   
   .. figure:: ./img/example-result.png
        :align: center

        ..





