# -*- coding: utf-8 -*-

import argparse
import os
import sys

import toml
import torch
import torch.nn as nn
import torch.nn.functional as F


class CONV2D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()

        op_list = [
            nn.Conv2d(
                in_channels=config["in_channels"],
                out_channels=config["out_channels"],
                kernel_size=config["kernel_size"],
                stride=config["stride"],
                padding=config["padding"],
                dilation=config["dilation"],
                groups=config["groups"],
                bias=config["bias"],
            )
        ]
        if config["activation_func"] == "ReLU":
            op_list.append(nn.ReLU())
        self.ops = nn.Sequential(*op_list)
        self.config = config

    def forward(self, inputs):
        output = self.ops(inputs)
        return output


class LINEAR_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()

        op_list = [
            nn.Linear(
                in_features=config["in_features"],
                out_features=config["out_features"],
                bias=config["bias"],
            )
        ]
        if config["activation_func"] == "ReLU":
            op_list.append(nn.ReLU())
        self.ops = nn.Sequential(*op_list)
        self.config = config

    def forward(self, inputs):
        output = self.ops(inputs)
        return output


class ADD2D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

    def forward(self, input1, input2):
        output = input1 + input2
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class ADD4D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

        if config["input0_is_weight"] and config["input_weight_shape"]:
            self.input0_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )
        elif config["input1_is_weight"] and config["input_weight_shape"]:
            self.input1_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )

    def forward(self, *args):
        input0 = None
        input1 = None
        if len(args) == 2:
            input0 = args[0]
            input1 = args[1]
        elif len(args) == 1 and hasattr(self, "input0_weight"):
            input0 = self.input0_weight
            input1 = args[0]
        elif len(args) == 1 and hasattr(self, "input1_weight"):
            input0 = args[0]
            input1 = self.input1_weight
        else:
            raise ValueError("Config of MatMul is error.")

        output = input0 + input1
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class SUB4D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

        if config["input0_is_weight"] and config["input_weight_shape"]:
            self.input0_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )
        elif config["input1_is_weight"] and config["input_weight_shape"]:
            self.input1_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )

    def forward(self, *args):
        input0 = None
        input1 = None
        if len(args) == 2:
            input0 = args[0]
            input1 = args[1]
        elif len(args) == 1 and hasattr(self, "input0_weight"):
            input0 = self.input0_weight
            input1 = args[0]
        elif len(args) == 1 and hasattr(self, "input1_weight"):
            input0 = args[0]
            input1 = self.input1_weight
        else:
            raise ValueError("Config of MatMul is error.")

        output = input0 - input1
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class MUL2D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

    def forward(self, input1, input2):
        output = input1 * input2
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class MUL4D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

        if config["input0_is_weight"] and config["input_weight_shape"]:
            self.input0_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )
        elif config["input1_is_weight"] and config["input_weight_shape"]:
            self.input1_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )

    def forward(self, *args):
        input0 = None
        input1 = None
        if len(args) == 2:
            input0 = args[0]
            input1 = args[1]
        elif len(args) == 1 and hasattr(self, "input0_weight"):
            input0 = self.input0_weight
            input1 = args[0]
        elif len(args) == 1 and hasattr(self, "input1_weight"):
            input0 = args[0]
            input1 = self.input1_weight
        else:
            raise ValueError("Config of MatMul is error.")

        output = input0 * input1
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class DIV_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

        if config["input0_is_weight"] and config["input_weight_shape"]:
            self.input0_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )
        elif config["input1_is_weight"] and config["input_weight_shape"]:
            self.input1_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )

    def forward(self, *args):
        input0 = None
        input1 = None
        if len(args) == 2:
            input0 = args[0]
            input1 = args[1]
        elif len(args) == 1 and hasattr(self, "input0_weight"):
            input0 = self.input0_weight
            input1 = args[0]
        elif len(args) == 1 and hasattr(self, "input1_weight"):
            input0 = args[0]
            input1 = self.input1_weight
        else:
            raise ValueError("Config of MatMul is error.")

        output = input0 / input1
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class EQUAL4D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()

    def forward(self, input1, input2):
        output = torch.eq(input1, input2)
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class GLOBAL_AVERAGE_POOLING_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.global_avg_pool = nn.AdaptiveAvgPool2d((1, 1))

    def forward(self, input):
        return self.global_avg_pool(input)


class AVERAGE_POOLING_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.avg_pool = nn.AvgPool2d(
            kernel_size=config["kernel_size"],
            stride=config["stride"],
            padding=config["padding"],
        )

    def forward(self, input):
        return self.avg_pool(input)


class MAX_POOLING_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        c = self.config["input_shape"][1]
        self.pre_max_pool = nn.Sequential(
            nn.Conv2d(c, c, kernel_size=3, padding=1), nn.ReLU()
        )
        self.max_pool = nn.MaxPool2d(
            kernel_size=config["kernel_size"],
            stride=config["stride"],
            padding=config["padding"],
        )

    def forward(self, input):
        x = self.pre_max_pool(input)
        x = self.max_pool(input)
        return x


class RESIZE2D_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["conv"]:
            op_list = [
                nn.Conv2d(
                    in_channels=config["in_features"],
                    out_channels=config["out_features"],
                    kernel_size=[1, 1],
                    stride=[1, 1],
                    padding=[0, 0],
                    dilation=[1, 1],
                    groups=1,
                    bias=True,
                ),
                nn.Upsample(scale_factor=2, mode="nearest"),
            ]
        else:
            op_list = [nn.Upsample(scale_factor=2, mode="nearest")]
        self.ops = nn.Sequential(*op_list)

    def forward(self, input):
        return self.ops(input)


class SIGMOID_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.sigmoid = nn.Sigmoid()

    def forward(self, input):
        return self.sigmoid(input)


class TANH_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.op = nn.Tanh()

    def forward(self, input):
        return self.op(input)


class LEAKYRELU_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.op = nn.LeakyReLU(config["slope"])

    def forward(self, input):
        return self.op(input)


class PRELU_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.op = nn.PReLU(config["num_parameters"])

    def forward(self, input):
        return self.op(input)


class HARDSIGMOID_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.op = nn.Hardsigmoid()

    def forward(self, input):
        return self.op(input)


class HARDSWISH_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.op = nn.Hardswish()

    def forward(self, input):
        return self.op(input)


class CONCAT_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input1, input2):
        return torch.cat([input1, input2], dim=self.config["axis"])


class CLIP_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, inputs):
        output = torch.clip(inputs, min=self.config["min"], max=self.config["max"])
        return output


class FLATTEN_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()

        self.flatten = nn.Flatten(config["start_dim"], config["end_dim"])
        self.config = config

    def forward(self, inputs):
        output = self.flatten(inputs)
        return output


class RESHAPE_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, inputs):
        output = torch.reshape(inputs, self.config["shape"])
        return output


class TRANSPOSE_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        return torch.permute(input, dims=self.config["perm"])


class SOFTMAX_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()

        self.op = nn.Softmax(config["dim"])
        self.config = config

    def forward(self, inputs):
        if self.config["relu"]:
            inputs = nn.ReLU()(inputs)
        output = self.op(inputs)
        return output


class SQUEEZE_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, inputs):
        if self.config["dim"]:
            output = torch.squeeze(inputs, self.config["dim"])
        else:
            output = torch.squeeze(inputs)
        return output


class UNSQUEEZE_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, inputs):
        output = torch.unsqueeze(inputs, self.config["dim"])
        return output


class EXP_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        return torch.exp(input)


class LOG_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        return torch.log(input)


class SQRT_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        return torch.sqrt(input)


class SLICE_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        input = nn.ReLU()(input)
        if self.config["dim"] == 4:
            return input[0:1, 1:20:2, 1:-1, :]
        elif self.config["dim"] == 3:
            return input[0:1, 1:9:3, 1:-1:2]
        elif self.config["dim"] == 2:
            return input[:, 10:-10]
        elif self.config["dim"] == 1:
            return input[0:2]


class PAD_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.pads = [int(i) for i in self.config["pads"]]
        print(self.pads)

    def forward(self, input):
        input = nn.ReLU()(input)
        return F.pad(input, self.pads, self.config["mode"], self.config["const_value"])


class MATMUL_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        if config["activation_func"] == "ReLU":
            self.act = nn.ReLU()
        if config["input1_is_weight"] and config["input_weight_shape"]:
            self.static_weight = nn.Parameter(
                torch.randn(size=config["input_weight_shape"])
            )

    def forward(self, input1, *args):
        # By applying squeeze, the input is transformed to adapt the dimensions of matmul.
        input1 = torch.squeeze(input1, 0)
        input2 = None
        if len(args) > 0:
            input2 = args[0]
            input2 = torch.squeeze(input2, 0)
        elif hasattr(self, "static_weight"):
            input2 = self.static_weight
        else:
            raise ValueError("Config of MatMul is error.")

        output = torch.matmul(input1, input2)
        if hasattr(self, "act"):
            output = self.act(output)
        return output


class SPLIT_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config

    def forward(self, input):
        input = nn.ReLU()(input)
        output = torch.split(
            input, self.config["split_size_or_sections"], self.config["dim"]
        )
        return output


class GATHER_TEST(nn.Module):
    def __init__(self, config):
        super().__init__()
        self.config = config
        self.indices = torch.LongTensor(self.config["indices"])

    def forward(self, input):
        input = torch.squeeze(input, 0)
        array_idx = [
            self.indices if self.config["axis"] == i else slice(dim)
            for i, dim in enumerate(input.shape)
        ]
        output = input[array_idx]
        return output


if __name__ == "__main__":
    print(f"Test {os.path.basename(sys.argv[0])} Module Start...")

    parser = argparse.ArgumentParser(description="Module Test")
    parser.add_argument(
        "-c", "--config", required=True, type=str, help="Config (*.toml)."
    )
    parser.add_argument(
        "-t", "--target", type=str, default="esp32p4", help="esp32p4 or esp32s3."
    )
    parser.add_argument(
        "-b", "--bits", type=int, default=8, help="the number of bits, support 8 or 16"
    )
    args = parser.parse_args()

    # get config
    config = toml.load(args.config)

    # get model
    conv2d_cfg = config["ops_test"]["conv2d"]["cfg"][0]
    add2d_cfg = config["ops_test"]["add2d"]["cfg"][0]
    add2d_relu_cfg = config["ops_test"]["add2d"]["cfg"][1]
    mul2d_cfg = config["ops_test"]["mul2d"]["cfg"][0]
    mul2d_relu_cfg = config["ops_test"]["mul2d"]["cfg"][1]
    global_average_pooling_cfg = config["ops_test"]["global_average_pooling"]["cfg"][0]
    average_pooling_cfg = config["ops_test"]["average_pooling"]["cfg"][0]
    resize2d_cfg = config["ops_test"]["resize2d"]["cfg"][0]
    conv2d = CONV2D_TEST(conv2d_cfg)
    add2d = ADD2D_TEST(add2d_cfg)
    add2d_relu = ADD2D_TEST(add2d_relu_cfg)
    mul2d = MUL2D_TEST(mul2d_cfg)
    mul2d_relu = ADD2D_TEST(mul2d_relu_cfg)
    global_average_pooling = GLOBAL_AVERAGE_POOLING_TEST(global_average_pooling_cfg)
    average_pooling = AVERAGE_POOLING_TEST(average_pooling_cfg)
    resize2d = RESIZE2D_TEST(resize2d_cfg)

    # get inputs
    conv2d_inputs = torch.randn(conv2d_cfg["input_shape"])
    add2d_inputs = [
        torch.randn(add2d_cfg["input_shape"][0]),
        torch.randn(add2d_cfg["input_shape"][1]),
    ]
    mul2d_inputs = [
        torch.randn(mul2d_cfg["input_shape"][0]),
        torch.randn(mul2d_cfg["input_shape"][1]),
    ]
    global_average_pooling_inputs = torch.randn(
        global_average_pooling_cfg["input_shape"]
    )
    average_pooling_inputs = torch.randn(average_pooling_cfg["input_shape"])
    resize2d_inputs = torch.randn(resize2d_cfg["input_shape"])
    # print network
    # summary(conv2d, input_data=conv2d_inputs, col_names=("input_size", "output_size", "num_params"), device=torch.device('cpu'))
    # forward
    conv2d_outputs = conv2d(conv2d_inputs)
    add2d_outputs = add2d(*add2d_inputs)
    add2d_relu_outputs = add2d_relu(*add2d_inputs)
    mul2d_outputs = mul2d(*mul2d_inputs)
    mul2d_relu_outputs = mul2d_relu(*mul2d_inputs)
    global_average_pooling_outputs = global_average_pooling(
        global_average_pooling_inputs
    )
    average_pooling_outputs = average_pooling(average_pooling_inputs)
    resize2d_outputs = resize2d(resize2d_inputs)

    print(f"Test {os.path.basename(sys.argv[0])} Module End...")
    pass
