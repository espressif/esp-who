# ESP-WHO [[English]](./README.md)

ESP-WHO 是基于乐鑫芯片的图像处理开发平台，其中包含了实际应用中可能出现的开发示例。

## 概述

ESP-WHO 提供了人脸检测、人脸识别、行人检测等示例。您可以基于这些示例，衍生出丰富的实际应用。ESP-WHO 的运行基于 ESP-IDF。[ESP-DL](https://github.com/espressif/esp-dl) 为 ESP-WHO 提供了丰富的深度学习相关接口，配合各种外设可以实现许多有趣的应用。

## 新功能
1. 该仓库已完全重构。适配新版 [ESP-DL](https://github.com/espressif/esp-dl)
2. 支持新芯片 [ESP32-P4](https://www.espressif.com/en/products/socs/esp32-p4)。
3. 摄像头和深度学习模型现在异步执行，可实现更高的帧率。
4. 添加 [lvgl](https://lvgl.io/)(轻量级多功能图形库) 支持，您可以自由开发自己的图形应用程序。
5. 添加了新的行人检测模型。

某些芯片（如 esp32 和 esp32-s2）以及示例（如猫脸检测、颜色检测、二维码识别）目前不在此分支中，还未适配完成。旧分支可在此处找到。
[老 ESP-WHO 分支](https://github.com/espressif/esp-who/tree/release/v1.1.0)

## 准备工作

### 硬件准备

我们推荐新手开发者使用乐鑫设计的开发板。ESP-WHO 提供的示例基于以下乐鑫开发板开发，开发板与芯片的对应关系如下表所示。
    
| 芯片 | [ESP32-S3](https://www.espressif.com/en/products/socs/esp32-s3) | [ESP32-P4](https://www.espressif.com/en/products/socs/esp32-p4) |
|-------------------|---------------------------------------------------|-----------------------------------------------------------------------------------|
| 开发板 | [ESP-S3-EYE](https://www.espressif.com/en/products/devkits) | [ESP32-P4-Function-EV-Board](https://www.espressif.com/en/products/devkits) |

> 使用上表中未提及的开发板，需要手动修改外设的管脚配置，例如摄像头、LCD 和按键等。

### 软件准备

#### 获取 ESP-IDF

ESP-WHO 在 [ESP-IDF release/v5.4](https://github.com/espressif/esp-idf/tree/release/v5.4) 分支上运行。有关获取 ESP-IDF 的详细信息，请参阅 [ESP-IDF 编程指南](https://idf.espressif.com/)。

#### 获取 ESP-WHO

在终端中运行以下命令下载 ESP-WHO：

```bash
git clone https://github.com/espressif/esp-who.git
```

## 运行示例

ESP-WHO 的所有示例都存放在 [examples](./examples) 中。该文件夹架构如下所示：

```bash
├── examples
│   ├── human_face_detect
│   │   ├── human_face_detect_lcd           // 使用esp_lcd的高帧率示例
│   │   └── human_face_detect_terminal      // 没有lcd的情况下在terminal显示结果
│   ├── human_face_recognition              // 使用lvgl的人脸识别示例
│   ├── multiple_detect                     // 多个检测模型同时运行的示例
│   └── pedestrian_detect
│       ├── pedestrian_detect_lcd
│       └── pedestrian_detect_terminal
```

对于[硬件准备](#硬件准备)中所提到的开发板，所有示例都是开箱即用的，要运行示例仅需执行[步骤 1：硬件连接](#步骤-1硬件连接)， [步骤 2：设置目标芯片](#步骤-2设置目标芯片)和[步骤 4：运行和监视](#步骤-4运行和监视)。

### 步骤 1：硬件连接

如果您使用的是 ESP32-P4-Function-EV-Board，请按照用户指南[ESP32-P4 用户指南](https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)将摄像头和液晶显示屏连接到开发板。

### 步骤 2：设置目标芯片

打开终端并转到存储示例的任何文件夹（例如 examples/human_face_detection）。运行以下命令设置目标芯片：

```bash
idf.py set-target [SoC]
```

将 [SoC] 替换为您的目标芯片，例如 esp32s3、esp32p4

### 步骤 3：（可选）更改 menuconfig 中的选项

除了默认配置外，示例中可能还有一些您可以自由修改的选项。有关更多详细信息，请阅读示例下的 README.md。

```bash
idf.py menuconfig
```

### 步骤 4：运行和监视

烧录程序，运行 IDF 监视器：

```bash
idf.py flash monitor
```

## 反馈

如果在使用中发现任何问题，请提交相关 [issue](https://github.com/espressif/esp-who/issues)，我们将尽快予以答复。
