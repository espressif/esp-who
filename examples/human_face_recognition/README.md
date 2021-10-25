# Human Face Recognition Example [[中文]](./README_ZH.md)

The following is the special configuration of face recognition examples. For general configuration, please refer to [this](../../README.md)


## Configure the face recognition model

Enter `idf.py menuconfig` in the terminal and click (Top) -> Component config -> ESP-WHO Configuration -> Model Configuration -> Face Recognition to enter the face recognition model configuration interface, as shown below:
![](../../img/face_recognition_model_config.png)
You can configure the version and quantification type of the model here.

## How to Use Example

- The interactive button is the Boot button.
- Short press the button: recognize the face captured by the camera at this time.
- Long press the button: enroll the face captured by the camera at this time.
- Double click the button: delete the last enrolled face.

