## Best Practice

### Step1: default quantization
A series of general quantization settings has been established to achieve a good quantization accuracy. Please start by using the following script to quantize your model:

- quantize_torch_model.py: quantize torch model by default quantization setting
- quantize_onnx_model.py: quantize onnx model by default quantization setting
- quantize_tf_model.py: convert tflite model to onnx model

### Step2: layerwise equalization quantization
If the model experiences significant accuracy loss after quantization, you can continue to use this method to reduce the error.   
This method relies on equalizing the weight ranges in the network by making use of a scale-equivariance property of activation functions,
which is proposed by [Data-Free Quantization Through Weight Equalization and Bias Correction](https://arxiv.org/abs/1906.04721).

### Step3: mixed precision quantization
The mixed-precision quantization here is a combination of 8-bit fixed-point and 16-bit fixed-point. Some layers may experience significant errors when quantized to 8-bit fixed-point, so you can dispatch some layers to 16-bit fixed-point to reduce quantization errors.

