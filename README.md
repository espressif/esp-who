# ESP-WHO [[中文]](./README_CN.md)

ESP-WHO is an image processing development platform based on Espressif chips. It contains development examples that may be applied in practical applications.

## Overview

ESP-WHO provides examples such as Human Face Detection, Human Face Recognition, Pedestrian Detection etc. You can develop a variety of practical applications based on these examples. ESP-WHO runs on ESP-IDF. [ESP-DL](https://github.com/espressif/esp-dl) provides rich deep learning related interfaces for ESP-WHO, which can be implemented with various peripherals to realize many interesting applications.

## What's new
1. The repository has been fully refactored. It adapts to the new [ESP-DL](https://github.com/espressif/esp-dl)
2. New chip [ESP32-P4](https://www.espressif.com/en/products/socs/esp32-p4) is now supported.
3. Camera and the deep learning model now runs asynchronously, which achieves higher fps.
4. Add [lvgl](https://lvgl.io/)(Light and Versatile Graphics Library) support, feel free to develop your own graphical applications.
5. New pedestrian detect model is added.

Some chip such as esp32 and esp32-s2, and examples such as cat face detection, color detection, code recognition is not available in this branch currently, we're still working on them. Old branch can be found here.
[old ESP-WHO branch](https://github.com/espressif/esp-who/tree/release/v1.1.0)

## What You Need

### Hardware

We recommend novice developers to use the development boards designed by Espressif. The examples provided by ESP-WHO are developed based on the following Espressif development board, and the corresponding relationships between the development boards and SoC are shown in the table below.
    
| Soc               | [ESP32-S3](https://www.espressif.com/en/products/socs/esp32-s3) | [ESP32-P4](https://www.espressif.com/en/products/socs/esp32-p4)             |
|-------------------|-----------------------------------------------------------------|-----------------------------------------------------------------------------|
| Development Board | [ESP-S3-EYE](https://www.espressif.com/en/products/devkits)     | [ESP32-P4-Function-EV-Board](https://www.espressif.com/en/products/devkits) |

> Using a development board not mentioned in the table above, configure pins assigned to peripherals manually, such as camera, LCD, and buttons.

### Software

#### Get ESP-IDF


ESP-WHO runs on [ESP-IDF release/v5.4](https://github.com/espressif/esp-idf/tree/release/v5.4) branch . For details on getting ESP-IDF, please refer to [ESP-IDF Programming Guide](https://idf.espressif.com/).  

#### Get ESP-WHO

Run the following commands in your terminal to download ESP-WHO:

```bash
git clone https://github.com/espressif/esp-who.git
```

## Run Examples

All examples of ESP-WHO are stored in [examples](./examples) folder. Structure of this folder is shown below:

```bash
├── examples
│   ├── human_face_detect
│   │   ├── human_face_detect_lcd           // high fps diaplay with esp_lcd
│   │   └── human_face_detect_terminal      // output in terminal witout lcd
│   ├── human_face_recognition              // human face recognition demo with lvgl
│   ├── multiple_detect                     // run mutiple detect models in the same time.
│   └── pedestrian_detect
│       ├── pedestrian_detect_lcd
│       └── pedestrian_detect_terminal
```

For the development boards mentioned in [Hardware](#Hardware), all examples are available out of the box. To run the examples, you only need to perform [Step 1: Hardware connection](#step-1-hardware-connection), [Step 2: Set the target chip](#step-2-set-the-target-chip) and [Step 4: Launch and monitor](#step-4-launch-and-monitor).


### Step 1: Hardware connection

If you are using ESP32-P4-Function-EV-Board, please following the user guide [ESP32-P4 user guide](https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html) to connect the camera and lcd to the development board.

### Step 2: Set the target chip

Open the terminal and go to any folder that stores examples (e.g. examples/human_face_detection). Run the following command to set the target chip: 

```bash
idf.py set-target [SoC]
```

Replace [SoC] with your target chip, e.g.  esp32s3, esp32p4


### Step 3: (Optional) change options in menuconfig

In addtion to the default configuration, there may be some options in the example that you can modify freely. For more details, read the README.md under the examples.

```bash
idf.py menuconfig
```

### Step 4: Launch and monitor

Flash the program and launch IDF Monitor:

```bash
idf.py flash monitor
```

## Feedback

Please submit an [issue](https://github.com/espressif/esp-who/issues) if you find any problems using our products, and we will reply as soon as possible.
