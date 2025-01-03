# Generate test cases for conv2d

```bash
python gen_test_cases.py --config=./config/op_cfg.toml --target esp32p4 --bits 8 --op conv2d --output-path ./test_cases
```

# Add op test cases
## 1. Implement torch model in `ops_test.py`
```
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
```


## 2. Add op test cases in `config/op_cfg.toml`
```
    [ops_test.conv2d]
    class_name = "CONV2D_TEST"
        [[ops_test.conv2d.cfg]]
        # conv2d, pointwise, aligned
        input_shape = [1, 3, 224, 224]
        export_name_prefix = "conv2d_s8_ishap_1_3_224_224_kshap_16_3_3_3"
        export_path = ""
        in_channels = 3
        out_channels = 16
        kernel_size = [3, 3]
        stride = [2, 2]
        padding = [0, 0]
        dilation = [1, 1]
        groups = 1
        bias = true
        activation_func = "ReLU"    # "", "ReLU"

        [[ops_test.conv2d.cfg]]
        # conv2d, pointwise, aligned
        input_shape = [1, 16, 32, 32]
        export_name_prefix = "conv2d_s8_ishap_1_16_32_32_kshap_32_16_1_1"
        export_path = ""
        in_channels = 16
        out_channels = 32
        kernel_size = [1, 1]
        stride = [1, 1]
        padding = [0, 0]
        dilation = [1, 1]
        groups = 1
        bias = true
        activation_func = ""    # "", "ReLU"
```
