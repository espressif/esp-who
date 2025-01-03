# ESP-DL

ESP-DL is designed to maintain optimal performance while significantly reducing the workload in model deployment. Our project has achieved the following key features:

### ESP-DL Standard Model Format

The ESP-DL standard model format is a binary format used to store the model graph, weights, and other essential information, with a file extension of `.espdl`. This format is similar to the ONNX model format but replaces ONNX's Protobuf with FlatBuffers, making our models more lightweight and supporting zero-copy deserialization. This feature ensures faster data access by eliminating the need to copy serialized data into separate memory areas.

### [esp-ppq](https://github.com/espressif/esp-ppq)

ESP-PPQ is a model quantization tool developed based on the open-source project PPQ. Users can select the ESP-DL target platform and directly export ESP-DL standard model files. ESP-PPQ inherits all the functionalities and documentation from the PPQ project, allowing users to conveniently choose quantization algorithms and analyze quantization errors.

### Efficient Operator Implementation

We have efficiently implemented common AI operators, including Conv2d, Pool2D, Gemm, Add, Mul, etc., based on AI instructions. These operators are precisely aligned with the PyTorch operator implementation, ensuring that the results obtained from the esp-ppq tool are consistent with those running on ESP-DL.

### Static Memory Planner

A new static memory planner is designed for the Internal RAM/PSRAM memory structure. Considering that internal RAM has faster access speed but limited capacity, we provide an API that allows users to customize the size of the internal RAM that the model can use. The memory planner will automatically allocate different layers to the optimal memory location based on the size of the internal RAM specified by the user, ensuring that the overall running speed is more efficient while occupying the minimum amount of memory.

### Dual Core Scheduling

The automatic dual-core scheduling enables computationally intensive operators to fully utilize the computing power of dual-cores. Currently, Conv2D and DepthwiseConv2D support dual-core scheduling. Below are some of our experimental results:

| |conv2d(input=224X224X3, kernel=3x3, output=112x112x16)|
|:---:|:---:|
|single core| 12.1.ms|
|dual core| 6.2 ms|

---

Explore ESP-DL to streamline your AI model deployment and achieve optimal performance with minimal resource usage.