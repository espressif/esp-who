# ESP-WHO [[中文]](./README_CN.md)

ESP-WHO is an image processing development platform based on Espressif chips. It contains development examples that may be applied in practical applications.

## Overview

ESP-WHO provides examples such as Human Face Detection, Human Face Recognition, Pedestrian Detection etc. You can develop a variety of practical applications based on these examples. ESP-WHO runs on ESP-IDF. [ESP-DL](https://github.com/espressif/esp-dl) provides rich deep learning related interfaces for ESP-WHO, which can be implemented with various peripherals to realize many interesting applications.

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
│   ├── human_face_detect_lvgl
│   ├── human_face_recognition
│   ├── pedestrian_detect
│   └── pedestrian_detect_lvgl  
```

For the development boards mentioned in [Hardware](#Hardware), all examples are available out of the box. To run the examples, you only need to perform [Step 1: Set the target chip] (#Step-1 Set the target chip) and [Step 4: Launch and monitor] (#Step-4 Launch and monitor).

### Step 1: Set the target chip

Open the terminal and go to any folder that stores examples (e.g. examples/human_face_detection/lcd). Run the following command to set the target chip: 

```bash
idf.py set-target [SoC]
```

Replace [SoC] with your target chip, e.g.  esp32s3, esp32p4


### Step 2: Launch and monitor

Flash the program and launch IDF Monitor:

```bash
idf.py flash monitor
```

## Feedback


Please submit an [issue](https://github.com/espressif/esp-who/issues) if you find any problems using our products, and we will reply as soon as possible.
