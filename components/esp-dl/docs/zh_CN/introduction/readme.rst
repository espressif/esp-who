***********
ESP-DL 简介
***********

:link_to_translation:`en:[English]`

ESP-DL 是一个专为 ESP 系列芯片设计的轻量级且高效的神经网络推理框架。通过 ESP-DL，您可以轻松快速地使用乐鑫的系统级芯片 (SoC) 开发 AI 应用。

概述
--------

ESP-DL 提供了加载、调试和运行 AI 模型的 API。该框架易于使用，并且可以与其他乐鑫 SDK 无缝集成。ESP-PPQ 作为 ESP-DL 的量化工具，能够量化来自 ONNX、Pytorch 和 TensorFlow 的模型，并将其导出为 ESP-DL 标准模型格式。

- **ESP-DL 标准模型格式**： 该格式类似于 ONNX，但使用 FlatBuffers 而不是 Protobuf，使其更轻量级并支持零拷贝反序列化，文件后缀为`.espdl`。

- **高效算子实现：** ESP-DL 高效地实现了常见的 AI 算子，如 Conv、Pool、Gemm、Add 和 Mul等。 `目前支持的算子 <../../../operator_support_state.md>`_

- **静态内存规划器：** 内存规划器根据用户指定的内部 RAM 大小，自动将不同层分配到最佳内存位置，确保高效的整体运行速度同时最小化内存使用。

- **双核调度：** 自动双核调度允许计算密集型算子充分利用双核计算能力。目前，Conv2D 和 DepthwiseConv2D 支持双核调度。

- **8bit LUT Activation：** 除了Relu, PRelu(n>1)之外的所有激活函数，ESP-DL 默认使用 8bit LUT(Look Up Table)方式实现,以加速推理。

ESP-DL系统框架图如下所示:

.. image:: ../../_static/architecture_cn.drawio.svg
    :alt: ESP-DL 架构
    :align: center

