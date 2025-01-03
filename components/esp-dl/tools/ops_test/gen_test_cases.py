# -*- coding: utf-8 -*-

import argparse
import importlib
import os
import sys
from typing import (
    Iterable,
)

import toml
import torch
from ppq import QuantizationSettingFactory
from ppq.api import espdl_quantize_torch
from ppq.quantization.optim import *
from torch.utils.data import DataLoader

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
torch.manual_seed(42)


class BaseInferencer:
    def __init__(
        self,
        model,
        export_path,
        target="esp32p4",
        num_of_bits=8,
        model_version="1.0",
        model_cfg=None,
        meta_cfg=None,
    ):
        self.model_cfg = model_cfg if model_cfg is not None else model.config
        self.model = model
        if not os.path.exists(export_path):
            os.makedirs(export_path)
        self.export_path = export_path

        # config device
        self.device_str = "cpu"
        self.calib_steps = meta_cfg["calib_steps"] if meta_cfg is not None else 32
        self.batch_size = meta_cfg["batch_size"] if meta_cfg is not None else 1

        self.input_shape = self.model_cfg["input_shape"]
        if not isinstance(self.input_shape[0], list):
            self.input_shape = [self.input_shape]
            self.multi_input = False
        else:
            self.multi_input = True

        # get calibration dataset.
        calibration_dataset = self.load_calibration_dataset()
        self.calib_dataloader = DataLoader(
            dataset=calibration_dataset,
            batch_size=self.batch_size,
            shuffle=False,
        )

        # get quantization config.
        self.num_of_bits = num_of_bits
        self.model_version = model_version
        self.target = target
        # self.input_dtype = torch.float if self.num_of_bits == 8 else torch.float64
        self.input_dtype = torch.float
        print(self.target)

    def __call__(self):
        # get the export files path
        name_prefix = (
            self.model_cfg["export_name_prefix"] + "_s" + str(self.num_of_bits)
        )
        espdl_export_file = os.path.join(self.export_path, name_prefix + ".espdl")

        collate_fn = (
            (lambda x: x.type(self.input_dtype).to(self.device_str))
            if not self.multi_input
            else (lambda x: [xx.type(self.input_dtype).to(self.device_str) for xx in x])
        )

        print("start PTQ")
        # create a setting for quantizing your network with ESPDL.
        quant_setting = QuantizationSettingFactory.espdl_setting()
        quant_setting.dispatcher = "allin"
        quant_ppq_graph = espdl_quantize_torch(
            model=self.model,
            espdl_export_file=espdl_export_file,
            calib_dataloader=self.calib_dataloader,
            calib_steps=self.calib_steps,
            input_shape=self.input_shape,
            target=self.target,
            num_of_bits=self.num_of_bits,
            collate_fn=collate_fn,
            setting=quant_setting,
            device=self.device_str,
            error_report=False,
            skip_export=False,
            export_test_values=True,
            export_config=True,
            verbose=1,
            int16_lut_step=1,
        )

    def load_calibration_dataset(self) -> Iterable:
        if not self.multi_input:
            return [
                torch.randn(size=self.input_shape[0][1:])
                for _ in range(self.batch_size)
            ]
        else:
            return [
                [torch.randn(size=i[1:]) for i in self.input_shape]
                for _ in range(self.batch_size)
            ]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="")
    parser.add_argument(
        "-c", "--config", required=True, type=str, help="Config (*.toml)."
    )
    parser.add_argument(
        "-o",
        "--output-path",
        required=True,
        type=str,
        default=None,
        help="Output Path.",
    )
    parser.add_argument(
        "-t",
        "--target",
        type=str,
        default="esp32p4",
        help="esp32p4 or esp32s3, (defaults: esp32p4).",
    )
    parser.add_argument(
        "-b",
        "--bits",
        type=int,
        default=8,
        help="the number of bits, support 8 or 16, (defaults: 8).",
    )
    parser.add_argument(
        "-v",
        "--version",
        type=str,
        default="v1.0",
        help="the version of the test case, (defaults: v1.0)",
    )
    parser.add_argument("--ops", nargs="+", type=str, help="An array of ops")
    args = parser.parse_args()

    # load config
    config = toml.load(args.config)
    op_test_config = config["ops_test"]

    # generate test cases
    pkg = importlib.import_module(op_test_config["class_package"])
    if args.ops:
        op_set = args.ops
    else:
        op_set = []
        for op_type in op_test_config:
            if op_type == "class_package":
                continue
            op_set.append(op_type)

    for op_type in op_set:
        op_configs = op_test_config[op_type]["cfg"]
        op_class_name = op_test_config[op_type]["class_name"]
        quant_bits = op_test_config[op_type].get("quant_bits", [])
        if (args.bits == 8 and "int8" in quant_bits) or (
            args.bits == 16 and "int16" in quant_bits
        ):
            export_path = os.path.join(args.output_path, op_type)
            for cfg in op_configs:
                print("Op Class Name: ", op_class_name, "Configs: ", cfg)
                op = getattr(pkg, op_class_name)(cfg)
                BaseInferencer(
                    op,
                    export_path=export_path,
                    target=args.target,
                    num_of_bits=args.bits,
                    model_version=args.version,
                    meta_cfg=config["meta"],
                )()
        else:
            print(
                f"Skip op: {op_type}, do not support quantization with {args.bits} bits."
            )
