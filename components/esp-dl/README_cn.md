# ESP-DL [[English]](./README.md)

[![Documentation Status](./docs/_static/doc_latest.svg)](https://docs.espressif.com/projects/esp-dl/zh_CN/latest/index.html)    [![Component Registry](https://components.espressif.com/components/espressif/esp-dl/badge.svg)](https://components.espressif.com/components/espressif/esp-dl)

ESP-DL 是一个专为 ESP 系列芯片设计的轻量级且高效的神经网络推理框架。通过 ESP-DL，您可以轻松快速地使用乐鑫的系统级芯片 (SoC) 开发 AI 应用。

## Overview

ESP-DL 提供了加载、调试和运行 AI 模型的 API。该框架易于使用，并且可以与其他乐鑫 SDK 无缝集成。ESP-PPQ 作为 ESP-DL 的量化工具，能够量化来自 ONNX、Pytorch 和 TensorFlow 的模型，并将其导出为 ESP-DL 标准模型格式。

- **ESP-DL 标准模型格式：** 该格式类似于 ONNX，但使用 FlatBuffers 而不是 Protobuf，使其更轻量级并支持零拷贝反序列化，文件后缀为`.espdl`。

- **高效算子实现：** ESP-DL 高效地实现了常见的 AI 算子，如 Conv、Pool、Gemm、Add 和 Mul等。目前[算子支持状态](./operator_support_state.md)

- **静态内存规划器：** 内存规划器根据用户指定的内部 RAM 大小，自动将不同层分配到最佳内存位置，确保高效的整体运行速度同时最小化内存使用。

- **双核调度：** 自动双核调度允许计算密集型算子充分利用双核计算能力。目前，Conv2D 和 DepthwiseConv2D 支持双核调度。

- **8bit LUT Activation：** 除了Relu, PRelu(n>1)之外的所有激活函数，ESP-DL 默认使用 8bit LUT(Look Up Table)方式实现,以加速推理。

## Getting Started

### 软件要求

- **ESP-IDF**  

ESP-DL 基于 ESP-IDF 运行。有关如何获取 ESP-IDF 的详细说明，请参阅 [ESP-IDF 编程指南](https://idf.espressif.com)。

> 请使用 [ESP-IDF](https://github.com/espressif/esp-idf) `release/v5.3` 或更高版本。

- **ESP-PPQ**

ESP-PPQ 是基于 ppq 的量化工具。如果你想量化自己的模型，请使用以下命令安装 esp-ppq：
```
pip uninstall ppq
pip install git+https://github.com/espressif/esp-ppq.git
```

### Model Quantization

ESP-PPQ 可以直接读取 ONNX 模型进行量化。Pytorch 和 TensorFlow 需要先转换为 ONNX 模型，因此请确保你的模型可以转换为 ONNX 模型。

我们提供了以下 Python 脚本模板。你可以根据你自己的模型选择合适的模板进行修改。更多详细信息请参阅 [使用 ESP-PPQ 量化模型](https://docs.espressif.com/projects/esp-dl/zh_CN/latest/tutorials/how_to_quantize_model.html)。  

[quantize_onnx_model.py](./tools/quantization/quantize_onnx_model.py): 量化 ONNX 模型

[quantize_torch_model.py](./tools/quantization/quantize_torch_model.py): 量化 Pytorch 模型

[quantize_tf_model.py](./tools/quantization/quantize_tf_model.py): 量化 TensorFlow 模型


### Model Deployment
ESP-DL 提供了一系列 API 来快速加载和运行模型。一个典型的示例如下：

```cpp
#include "dl_model_base.hpp"

extern const uint8_t espdl_model[] asm("_binary_model_name_espdl_start");
Model *model = new Model((const char *)espdl_model, fbs::MODEL_LOCATION_IN_FLASH_RODATA);
model->run(inputs); // inputs 是一个张量或张量映射
```

更多详细信息，请参阅 [使用 ESP-DL 加载模型](https://docs.espressif.com/projects/esp-dl/zh_CN/latest/tutorials/how_to_load_model.html) 和 [mobilenet_v2 示例](./examples/mobilenet_v2/)。


## Support models

[行人检测](./models/pedestrian_detect/)     
[人脸检测](./models/human_face_detect/)     
[人脸识别](./models/human_face_recognition/)     
[Imagenet 分类](./models/imagenet_cls/)    

## Suport Operators

如果你有遇到不支持的算子，请将问题在[issues](https://github.com/espressif/esp-dl/issues)中反馈给我们，我们会尽快支持。  
也欢迎大家贡献新的算子, 具体方法请参考[创建新模块（算子）](https://docs.espressif.com/projects/esp-dl/zh_CN/latest/tutorials/how_to_add_a_new_module%28operator%29.html)。

[算子支持状态](./operator_support_state.md)