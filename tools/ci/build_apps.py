from idf_build_apps import build_apps, find_apps
import argparse
import sys


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build all the apps.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("paths", nargs="+", help="Paths to the apps to build.")
    args = parser.parse_args()
    apps = find_apps(
        args.paths,
        target="all",
        #  recursive=True,
        default_build_targets=["esp32s3", "esp32p4"],
        build_dir=f"build_@n_@v_@t_@w",
        config_rules="sdkconfig.*=",
        # build_log_filename="build_log.txt",
        size_json_filename="size.json",
    )
    ret_code = build_apps(apps, copy_sdkconfig=True)
    sys.exit(ret_code)
