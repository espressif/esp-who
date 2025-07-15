# ESP-WHO [[中文]](./README_CN.md)

ESP-WHO is an image processing development platform based on Espressif chips. It contains development examples that may be applied in practical applications.

## Overview

ESP-WHO provides examples such as Human Face Detection, Human Face Recognition, Pedestrian Detection, QRCode Rocognition etc. You can develop a variety of practical applications based on these examples. ESP-WHO is developed based on [ESP-DL](https://github.com/espressif/esp-dl). It can realize many interesting applications with various peripherals.

## What's new
1. The repository has been fully refactored. It adapts to the new [ESP-DL](https://github.com/espressif/esp-dl)
2. New chip [ESP32-P4](https://www.espressif.com/en/products/socs/esp32-p4) is now supported.
3. Camera and the deep learning model now runs asynchronously, which achieves higher fps.
4. Add [lvgl](https://lvgl.io/)(Light and Versatile Graphics Library) support, feel free to develop your own graphical applications.
5. New pedestrian detect model is added.

Some chip such as esp32 and esp32-s2, and examples such as cat face detection, color detection is not available in this branch currently, we're still working on them. Old branch can be found here.
[old ESP-WHO branch](https://github.com/espressif/esp-who/tree/release/v1.1.0)

## Supported ESP-IDF version

| ESP-IDF <br> [Release/v5.4](https://github.com/espressif/esp-idf/tree/release/v5.4) | ESP-IDF <br> [Release/v5.5](https://github.com/espressif/esp-idf/tree/release/v5.5) |
|-------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------|
| :heavy_check_mark:                                                                  | :heavy_check_mark:                                                                  |

## Supported develop board

| Board name | SoC | Supported Features | Photo |
|:----------:|:---:|:-------------------|:-----:|
| [ESP32-P4 Function EV Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html) | esp32p4 | :musical_note: Audio <br/>:microphone: Audio Microphone  (es8311)<br/>:speaker: Audio Speaker  (es8311)<br/>:pager: LCD Display  (ek79007, ili9881c, lt8912b)<br/>:floppy_disk: uSD Card <br/>:point_up: Display Touch  (gt911)<br/> | <img src="https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/_images/esp32-p4-function-ev-board-isometric_v1.5.2.png" width="150"> |
| [ESP32-S3-EYE](docs/en/get-started/ESP32-S3-EYE_Getting_Started_Guide.md) | esp32s3 | :musical_note: Audio <br/>:microphone: Audio Microphone <br/>:radio_button: Button <br/>:camera: Camera <br/>:pager: LCD Display  (st7789)<br/>:video_game: IMU <br/>:floppy_disk: uSD Card <br/> | <img src="docs/_static/get-started/ESP32-S3-EYE-isometric.png" width="150"> |
| [ESP32-S3-Korvo-2](https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/dev-boards/user-guide-esp32-s3-korvo-2.html) | esp32s3 | :musical_note: Audio <br/>:microphone: Audio Microphone  (es7210)<br/>:speaker: Audio Speaker  (es8311)<br/>:radio_button: Button <br/>:camera: Camera <br/>:pager: LCD Display  (ili9341)<br/>:bulb: LED <br/>:floppy_disk: uSD Card <br/>:point_up: Display Touch  (tt21100)<br/> | <img src="https://docs.espressif.com/projects/esp-adf/en/latest/_images/esp32-s3-korvo-2-v3.0-overview.png" width="150"> |


## Quick Start

All examples of ESP-WHO are stored in [examples](./examples). Enter the corresponding folder and perform the following steps.

### Add environment variable

Linux:
```
export IDF_EXTRA_ACTIONS_PATH=/path_to_esp-who/tools/
echo $IDF_EXTRA_ACTIONS_PATH
```

Win/powershell:
```
$Env:IDF_EXTRA_ACTIONS_PATH="/path_to_esp-who/tools/"
echo $Env:IDF_EXTRA_ACTIONS_PATH
```

Win/cmd:
```
set IDF_EXTRA_ACTIONS_PATH=/path_to_esp-who/tools/
echo %IDF_EXTRA_ACTIONS_PATH%
```

> [!IMPORTANT]
> Make sure the echo command return the correct path.

### Set the target platform, generate and configure sdkconfig

#### (Optional) Cleanup
remove
- sdkconfig
- dependencies.lock.*
- build/
- managed_components/

#### Set the target SOC and the default sdkconfig configuration file.
```
idf.py reconfigure -DIDF_TARGET=target -DSDKCONFIG_DEFAULTS=sdkconfig.bsp.bsp_name
```

Add "" if using powershell.
```
idf.py reconfigure -DIDF_TARGET="target" -DSDKCONFIG_DEFAULTS="sdkconfig.bsp.bsp_name"
```

> [!NOTE]
> - The reconfigure process will create the sdkconfig file. However, if sdkconfig already exists, it will not be overwritten. So it is recommended to remove the sdkconfig file before run reconfigure.
> - Check the sdkconfig.bsp.* files under each example to see the supported bsp_name.

#### (Optional) Configure sdkconfig options
```
idf.py menuconfig
```

### Flash and monitor

```
idf.py [-p port] flash monitor
```

> [!NOTE]
> - Here [-p port] is optional, if no port is specified, all ports will be scanned. 
> - [check port on linux/macos](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/establish-serial-connection.html#check-port-on-linux-and-macos)  
> - [check port on win](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/establish-serial-connection.html#check-port-on-windows)

## Resources

* If you need to deploy your own model, you can refer to [ESP-DL](https://github.com/espressif/esp-dl), [ESP-DETECTION](https://github.com/espressif/esp-detection).

* For the use of the peripheral interfaces related to the development board, you can refer to [ESP-BSP](https://github.com/espressif/esp-bsp).
* For camera driver related information, please refer to [ESP32_CAMERA](https://github.com/espressif/esp32-camera), [ESP_VIDEO_COMPONENTS](https://github.com/espressif/esp-video-components).
* If you find an error or need new features during use, please check [GitHub Issues](https://github.com/espressif/esp-who/issues) first to ensure that the issue is not submitted repeatedly.
