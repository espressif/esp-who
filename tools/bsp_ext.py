# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from re import match, compile
from pathlib import Path
from click.core import Context
from typing import List
from idf_py_actions.tools import _parse_cmakecache
import sys


def action_extensions(base_actions, project_path=os.getcwd()):
    """Describes extension for Board Support Packages."""

    try:
        from idf_py_actions.tools import PropertyDict, red_print
    except ImportError:
        PropertyDict = dict
        red_print = print
    try:
        import ruamel.yaml
    except ImportError:
        red_print("ruamel.yaml package is not installed. No BSP extension is added!")
        return {}

    # List of supported BSPs
    BSPS = {
        "esp32_s3_eye",
        "esp32_s3_korvo_2",
        "esp32_p4_function_ev_board",
    }
    BSPS = BSPS | {bsp + "_noglib" for bsp in BSPS}

    BSP2IDF_TARGET = {
        "esp32_s3_eye": "esp32s3",
        "esp32_s3_korvo_2": "esp32s3",
        "esp32_p4_function_ev_board": "esp32p4",
    }
    BSP2IDF_TARGET.update({k + "_noglib": v for k, v in BSP2IDF_TARGET.items()})

    # List of supported DETECT_MODELS
    DETECT_MODELS = {
        "human_face_detect",
        "pedestrian_detect",
        "cat_detect",
        "dog_detect",
    }

    EXAMPLES = {"human_face_recognition", "object_detect", "qrcode_recognition"}

    def get_value_from_cache_or_env(key, global_args):
        regex = compile(rf"^{key}=.*")
        for entry in global_args.define_cache_entry:
            if match(regex, entry):
                return entry.split("=")[-1]
        cache_path = os.path.join(global_args.build_dir, "CMakeCache.txt")
        cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}
        if key in cache:
            return cache[key]
        return os.environ.get(key)

    def is_key_in_cache_file(key, global_args):
        cache_path = os.path.join(global_args.build_dir, "CMakeCache.txt")
        cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}
        return key in cache

    def is_key_in_cache(key, global_args):
        regex = compile(rf"^{key}=.*")
        for entry in global_args.define_cache_entry:
            if match(regex, entry):
                return True
        return is_key_in_cache_file(key, global_args)

    def global_callback(ctx: Context, global_args: PropertyDict, tasks: List) -> None:
        bsp = None
        sdkconfig_defaults = None
        detect_model = None
        idf_target = None
        for task in tasks:
            if task.name in ["set-target", "reconfigure", "all"]:
                example = os.path.basename(global_args["project_dir"])
                if example in EXAMPLES:
                    if task.name == "set-target":
                        idf_target = task.action_args["idf_target"]
                    else:
                        idf_target = get_value_from_cache_or_env(
                            "IDF_TARGET", global_args
                        )
                    if idf_target is None:
                        red_print(
                            "IDF_TARGET is not set. Add -DIDF_TARGET=xxx in idf.py command or set a environment variable."
                        )
                        sys.exit(2)
                    sdkconfig_defaults = get_value_from_cache_or_env(
                        "SDKCONFIG_DEFAULTS", global_args
                    )
                    if sdkconfig_defaults is None:
                        red_print(
                            "SDKCONFIG_DEFAULTS is not set. Add -DSDKCONFIG_DEFAULTS=xxx in idf.py command or set a environment variable."
                        )
                        sys.exit(2)
                    bsp = sdkconfig_defaults.split("sdkconfig.bsp.")[-1]
                    if bsp not in BSPS:
                        red_print(f"Invalid bsp: {bsp}, supported list: {BSPS}")
                        sys.exit(2)
                    if BSP2IDF_TARGET[bsp] != idf_target:
                        red_print(f"BSP {bsp} does not match idf_target {idf_target}.")
                        sys.exit(2)
                    if not is_key_in_cache("BSP", global_args):
                        global_args["define_cache_entry"].append(f"BSP={bsp}")
                    if example == "object_detect":
                        detect_model = get_value_from_cache_or_env(
                            "DETECT_MODEL", global_args
                        )
                        if detect_model is None:
                            red_print(
                                "DETECT_MODEL is not selected. Add -DDETECT_MODEL=xxx in idf.py command or set a environment variable."
                            )
                            sys.exit(2)
                        if detect_model not in DETECT_MODELS:
                            red_print(
                                f"Invalid detect_model: {detect_model}, supported list: {DETECT_MODELS}"
                            )
                            sys.exit(2)
                        if not is_key_in_cache("DETECT_MODEL", global_args):
                            global_args["define_cache_entry"].append(
                                f"DETECT_MODEL={detect_model}"
                            )

        if not is_key_in_cache_file("BSP", global_args) or not is_key_in_cache_file(
            "DETECT_MODEL", global_args
        ):
            tasks.insert(
                0,
                ctx.invoke(
                    ctx.command.get_command(ctx, "edit-manifest"),
                    bsp=bsp,
                    detect_model=detect_model,
                ),
            )

    def component_short_name(name):
        return name.split("/")[-1]

    def edit_manifest_component(manifest, component_set, component):
        if not component:
            return

        if manifest:
            for dep in list(manifest["dependencies"]):
                if component_short_name(dep) in component_set:
                    del manifest["dependencies"][dep]
            # Add the one we need
            manifest["dependencies"][component] = {"version": "*"}
        else:
            manifest["dependencies"] = {component: {"version": "*"}}

    def edit_manifest(
        action: str, ctx: Context, args: PropertyDict, **action_args: str
    ) -> None:
        manifest_path = Path(args["project_dir"]) / "main" / "idf_component.yml"
        yaml = ruamel.yaml.YAML()
        manifest = yaml.load(manifest_path)
        if manifest is None:
            manifest = {}

        edit_manifest_component(manifest, BSPS, action_args["bsp"])
        edit_manifest_component(manifest, DETECT_MODELS, action_args["detect_model"])
        yaml.dump(manifest, manifest_path)

    extensions = {
        "global_action_callbacks": [global_callback],
        "actions": {
            "edit-manifest": {
                "callback": edit_manifest,
                "help": "Edit idf_component.yml",
                "options": [
                    {"names": ["--bsp"], "help": "Add bsp component."},
                    {
                        "names": ["--detect-model"],
                        "help": "Add detect model component.",
                    },
                ],
                "hidden": True,
            },
        },
    }

    return extensions
