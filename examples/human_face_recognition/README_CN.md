# Human Face Recognition Example [[English]](./README.md)

如何运行示例可参考此[说明](../../README_CN.md)，以下为使用人脸识别示例的特殊说明。

## 人脸识别模型配置

在终端输入 `idf.py menuconfig` ，依次 (Top) -> Component config -> ESP-WHO -> Model Configuration -> Face Recognition 可进入人脸识别模型配置界面，如下图所示：

![](../../img/face_recognition_model_config.png)

您可以在这里配置模型的版本和量化方式。

## 按键默认设置

- 交互按键为Boot键。
- 短按按键：识别人脸。
- 长按按键：录入人脸。
- 双击按键：删除最后一个被录入的人脸。
