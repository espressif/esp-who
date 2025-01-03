import argparse
import re
from datetime import datetime
from pathlib import Path

import toml
from tabulate import tabulate


def camel_to_snake(name):
    if name == "PRelu":
        return "prelu"
    if name == "MatMul":
        return "matmul"

    s1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", name)
    return re.sub("([a-z0-9])([A-Z])", r"\1_\2", s1).lower()


def create_md(config_file, output_path):
    op_test_config = toml.load(config_file)["ops_test"]

    data = []
    yes_icon = "&#10004;"
    no_icon = "&#10006;"
    onnx_link = "[(ONNX)](https://onnx.ai/onnx/operators/onnx__##.html)"
    espdl_link = "[(ESP-DL)](esp-dl/dl/module/include/dl_module_##.hpp)"

    for op_type in op_test_config:
        if op_type == "class_package":
            continue
        # print(op_type, op_test_config[op_type]["quant_bits"], op_test_config[op_type]["support_state"])
        quant_bits = op_test_config[op_type].get("quant_bits", [])
        description = op_test_config[op_type].get("description", "")

        onnx_op_link = onnx_link.replace("##", op_type)
        espdl_op_link = espdl_link.replace("##", camel_to_snake(op_type))

        item = [op_type + espdl_op_link + onnx_op_link]
        if "int8" in quant_bits:
            item.append(yes_icon)
        else:
            item.append(no_icon)

        if "int16" in quant_bits:
            item.append(yes_icon)
        else:
            item.append(no_icon)

        item.append(description)
        data.append(item)

    sorted_op_list = sorted(data, key=lambda x: x[0].lower())
    sorted_op_list = [["Operator", "int8", "int16", "Description"]] + sorted_op_list
    markdown_table = tabulate(sorted_op_list, headers="firstrow", tablefmt="github")

    content = """# Operator Support State

| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |

## Quantization Strategy

The quantization type of all operators is symmetric quantization. Now ESP-DL supports both 8-bit and 16-bit.
The rounding for ESP32-S3 is [rounding half up](https://simple.wikipedia.org/wiki/Rounding#Round_half_up).
The rounding for ESP32-P4 is [rounding half to even](https://simple.wikipedia.org/wiki/Rounding#Round_half_to_even).

## Support Operators

The ESP-DL operator interface is aligned with ONNX. The opset 13 is recommended to export ONNX.
Currently, the following {op_num} operators have been implemented and tested. Some operators do not implement all functionalities and attributes. Please refer to the description of each operator or [test cases]({config_file}) for details.
"""

    current_time = datetime.now()
    formatted_time = current_time.strftime("%Y-%m-%d %H:%M:%S")

    content = (
        content.format(
            op_num=len(data), config_file="./tools/ops_test/config/op_cfg.toml"
        )
        + markdown_table
        + f"\n\nGeneration Time: {formatted_time}"
    )
    filename = "operator_support_state.md"

    with open(Path(output_path) / filename, "w") as f:
        f.write(content)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="")
    parser.add_argument(
        "-c",
        "--config",
        default="./config/op_cfg.toml",
        type=str,
        help="Config (*.toml).",
    )
    parser.add_argument(
        "-o",
        "--output-path",
        type=str,
        default="../../",
        help="Output Path.",
    )

    args = parser.parse_args()
    create_md(args.config, args.output_path)
