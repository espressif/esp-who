# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

"""
This file is used in CI generate binary files for different kinds of apps
"""

import argparse
import logging
import os
import re
import shutil
import sys
from pathlib import Path

from idf_build_apps import build_apps, find_apps, setup_logging

LOGGER = logging.getLogger("idf_build_apps")
current_dir = os.path.dirname(os.path.abspath(__file__))

PROJECT_ROOT = Path(__file__).parent.parent.absolute()
sys.path.append(str(PROJECT_ROOT / "esp-dl" / "fbs_loader"))
print(PROJECT_ROOT / "esp-dl" / "fbs_loader")
from pack_espdl_models import pack_models  # noqa: E402

APPS_BUILD_PER_JOB = 5
IGNORE_WARNINGS = [
    r"Wunused-variable",
    r"Wstrict-aliasing",
    r"Wformat=",
    r"Warray-bounds",
    r"Wdeprecated-copy",
    r"Wignored-qualifiers",
    r"#pragma once in main file",
    r"Wunused-but-set-variable",
    r"-fpermissive",
    r"Wmissing-field-initializers",
]


def list_directories(path):
    directories = []

    try:
        with os.scandir(path) as entries:
            for entry in entries:
                if entry.is_dir():
                    directories.append(entry)
        return directories
    except FileNotFoundError:
        LOGGER.error(f"The {path} directory is not exist.")
        return None


def generate_model_config(
    target, model_file, config_file, key="CONFIG_MODEL_FILE_PATH"
):
    model_prefix = os.path.splitext(os.path.basename(model_file))[0]
    new_config_name = f"sdkconfig.model.{target}_{model_prefix}"
    new_config_file = os.path.join(os.path.dirname(config_file), new_config_name)
    d = {}

    with open(config_file) as fr:
        for line in fr:
            m = re.compile(r"^([^=]+)=([^\n]*)\n*$").match(line)
            if not m:
                continue
            d[m.group(1)] = m.group(2)

    d[key] = f'"{(os.path.abspath(model_file))}"'
    with open(new_config_file, "w+") as f:
        for key, value in d.items():
            f.write(f"{key}={value}\n")

    return new_config_file


def _get_idf_version():
    if os.environ.get("IDF_VERSION"):
        return os.environ.get("IDF_VERSION")
    version_path = os.path.join(os.environ["IDF_PATH"], "tools/cmake/version.cmake")
    regex = re.compile(r"^\s*set\s*\(\s*IDF_VERSION_([A-Z]{5})\s+(\d+)")
    ver = {}
    with open(version_path) as f:
        for line in f:
            m = regex.match(line)
            if m:
                ver[m.group(1)] = m.group(2)
    return "{}.{}".format(int(ver["MAJOR"]), int(ver["MINOR"]))


def get_cmake_apps(
    paths,
    target,
    config_rules_str,
    default_build_targets,
    model_path=None,
):  # type: (List[str], str, List[str]) -> List[App]
    idf_ver = _get_idf_version()
    if not model_path or not os.path.exists(model_path):
        apps = find_apps(
            paths,
            recursive=True,
            target=target,
            build_dir=f"{idf_ver}/build_@t_@w",
            config_rules_str=config_rules_str,
            check_warnings=True,
            preserve=True,
            default_build_targets=default_build_targets,
        )
    else:
        if target == "all":
            target_list = ["esp32s3", "esp32p4"]
        else:
            target_list = [target]

        for t in target_list:
            # model_files = list_directories(model_path)
            target_model_path = os.path.join(model_path, t)
            model_files = list_directories(target_model_path)
            if not model_files or len(model_files) == 0:
                continue
            LOGGER.info(f"Read models from {target_model_path}")

            config_file_path = os.path.join(paths[0], f"sdkconfig.defaults.{t}")
            if not os.path.exists(config_file_path):
                LOGGER.error(f"Please add {config_file_path}")
            for model_file in model_files:
                generate_model_config(t, model_file, config_file_path)

        apps = find_apps(
            paths,
            recursive=True,
            target=target,
            build_dir=f"{idf_ver}/build_@w",
            config_rules_str="sdkconfig.model.*=",
            check_warnings=True,
            preserve=True,
            default_build_targets=default_build_targets,
        )

        # generate all model config

    return apps


def build_and_copy(apps_to_build, model_path):
    target_apps = {}
    model_path = Path(model_path)
    for app in apps_to_build:
        if app.target not in target_apps:
            target_apps[app.target] = Path(app.build_path)

    for target, build_path in target_apps.items():
        basedir = build_path.parent
        target_model_path = model_path / target
        opset = [subdir for subdir in target_model_path.iterdir() if subdir.is_dir()]
        for op_path in opset:
            op_build_path = basedir / f"build_{target}_{op_path.name}"
            if op_build_path.exists():
                shutil.rmtree(op_build_path)
                shutil.copytree(build_path, op_build_path)
            else:
                shutil.copytree(build_path, op_build_path)
            print(build_path, op_build_path, op_path)
            bin_path = op_build_path / "espdl_models" / "models.espdl"
            pack_models(op_path, bin_path)


def main(args):  # type: (argparse.Namespace) -> None
    default_build_targets = (
        args.default_build_targets.split(",") if args.default_build_targets else None
    )
    idf_ver = _get_idf_version()
    # find app
    apps = find_apps(
        args.paths,
        recursive=True,
        target=args.target,
        build_dir=f"{idf_ver}/build_@t_@w",
        config_rules_str=args.config,
        check_warnings=True,
        preserve=True,
        default_build_targets=default_build_targets,
    )
    print(apps, len(apps))
    if args.exclude_apps:
        apps_to_build = [app for app in apps if app.name not in args.exclude_apps]
    else:
        apps_to_build = apps[:]

    LOGGER.info("Found %d apps after filtering", len(apps_to_build))
    LOGGER.info(
        "Suggest setting the parallel count to %d for this build job",
        len(apps_to_build) // APPS_BUILD_PER_JOB + 1,
    )

    ret_code = build_apps(
        apps_to_build,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        collect_size_info=args.collect_size_info,
        keep_going=True,
        ignore_warning_strs=IGNORE_WARNINGS,
        copy_sdkconfig=True,
    )
    if args.model_path and os.path.exists(args.model_path):
        build_and_copy(apps_to_build, args.model_path)

    sys.exit(ret_code)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build all the apps for different test types. Will auto remove those non-test apps binaries",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("paths", nargs="*", help="Paths to the apps to build.")
    parser.add_argument(
        "-t",
        "--target",
        default="all",
        help='Build apps for given target. could pass "all" to get apps for all targets',
    )
    parser.add_argument(
        "--config",
        default=["sdkconfig.ci=default", "sdkconfig.ci.*=", "=default"],
        action="append",
        help="Adds configurations (sdkconfig file names) to build. This can either be "
        "FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, "
        "relative to the project directory, to be used. Optional NAME can be specified, "
        "which can be used as a name of this configuration. FILEPATTERN is the name of "
        "the sdkconfig file, relative to the project directory, with at most one wildcard. "
        "The part captured by the wildcard is used as the name of the configuration.",
    )
    parser.add_argument(
        "--parallel-count", default=1, type=int, help="Number of parallel build jobs."
    )
    parser.add_argument(
        "--parallel-index",
        default=1,
        type=int,
        help="Index (1-based) of the job, out of the number specified by --parallel-count.",
    )
    parser.add_argument(
        "--collect-size-info",
        type=argparse.FileType("w"),
        help="If specified, the test case name and size info json will be written to this file",
    )
    parser.add_argument(
        "--exclude-apps",
        nargs="*",
        help="Exclude build apps",
    )
    parser.add_argument(
        "--default-build-targets",
        default=None,
        help="default build targets used in manifest files",
    )
    parser.add_argument(
        "-m",
        "--model-path",
        default=None,
        help="model path used in test",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        default=0,
        help="Show verbose log message",
    )

    arguments = parser.parse_args()
    if not arguments.paths:
        arguments.paths = [PROJECT_ROOT]
    setup_logging(verbose=arguments.verbose)  # Info
    main(arguments)
