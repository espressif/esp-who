import os
import subprocess

TF_PATH = "./models/tf/"

# ------------------------------------------
#  Convert Tensorflow Model to ONNX
# ------------------------------------------

# Using tf2onnx to convert a tflite model to ONNX format
# Install tf2onnx by "pip install tf2onnx"
# Please refer to https://onnxruntime.ai/docs/tutorials/tf-get-started.html for more details

os.makedirs(TF_PATH, exist_ok=True)
tflite_model_path = os.path.join(TF_PATH, "mobilenet_v2_1.0_224.tflite")
onnx_model_path = os.path.join(TF_PATH, "mobilenet_v2_1.0_224.onnx")
subprocess.run(
    [
        "python",
        "-m",
        "tf2onnx.convert",
        "--tflite",
        tflite_model_path,
        "--output",
        onnx_model_path,
        "--opset",
        13,
    ]
)

# Note: the inputs of this model is [1, 224, 224, 3]

# -------------------------------------------
# Please use `quantize_onnx_model.py` to quantize ONNX model
# --------------------------------------------
