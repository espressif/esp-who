# Human Face Recognition [[中文]](./README_CN.md)

This example demonstrates how to recognize human faces captured by the camera of an Espressif development board in real time. The output results of this example can be shown on an LCD screen or in your terminal.


## Run the Example

Follow steps in [ESP-WHO README](../../README.md) to run the example with default setting. 

## Configure Face Recognition Model

In addition to default setting, this example allows you to configure model version and quantization type.

Before Step 4: Flash and Monitor in [ESP-WHO README](../../README.md), run `idf.py menuconfig` in your terminal, go to the face recognition model configuration interface as shown below by entering (Top) -> Component config -> ESP-WHO Configuration -> Model Configuration -> Face Recognition, and configure model version and quantization type:
![](../../img/face_recognition_model_config.png)


You can also configure the size of fr partition in partitions.csv to adjust number of faces to be stored in flash.
The default partition size is 128K. You will be able to see information displayed as below:
```
I (1070) MFN: fr partition size: 131072 bytes, maxminum 62 IDs can be stored
```

## Use the Example

You can use the Boot button on the development board for interaction.

- Short press the button: recognize the face captured by the camera in real time.
- Long press the button: enroll the face captured by the camera in real time.
- Double click the button: delete the last enrolled face.

