# Human Face Recognition Example [[English]](./README.md)

以下为使用人脸识别示例的特殊配置，若要进行通用配置可参考此[说明](../../README_ZH.md)。

## 人脸识别模型配置

在终端输入 `idf.py menuconfig` ，依次 (Top) -> Component config -> ESP-WHO Configuration -> Model Configuration -> Face Recognition 可进入人脸识别模型配置界面，如下图所示：

![](../../img/face_recognition_model_config.png)

您可以在这里配置模型的版本和量化方式。

## 如何使用示例

- 交互按键为Boot键。
- 短按按键：识别此时摄像头拍到的人脸。
- 长按按键：录入此时摄像头拍到的人脸。
- 双击按键：删除最后一个被录入的人脸。
