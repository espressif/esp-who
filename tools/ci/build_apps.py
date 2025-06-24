from idf_build_apps import build_apps, find_apps
import argparse
import sys

BSP2TARGET = {
    "esp_wrover_kit": "esp32",
    "esp32_azure_iot_kit": "esp32",
    "esp32_s2_kaluga_kit": "esp32s2",
    "esp32_s3_eye": "esp32s3",
    "esp32_s3_lcd_ev_board": "esp32s3",
    "esp32_s3_usb_otg": "esp32s3",
    "esp-box": "esp32s3",
    "esp32_s3_korvo_2": "esp32s3",
    "esp-box-lite": "esp32s3",
    "esp32_lyrat": "esp32",
    "esp-box-3": "esp32s3",
    "esp32_c3_lcdkit": "esp32c3",
    # "esp_bsp_generic",
    # "esp_bsp_devkit",
    "esp32_s3_korvo_1": "esp32s3",
    "esp32_p4_function_ev_board": "esp32p4",
    "m5stack_core_s3": "esp32s3",
    "m5dial": "esp32s3",
    "m5stack_core_2": "esp32",
    # "m5stack_core" : "",
    # "m5_atom_s3",
}
BSP2TARGET.update({key + "_noglib": value for key, value in BSP2TARGET.items()})

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build all the apps.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("paths", nargs="+", help="Paths to the apps to build.")
    parser.add_argument("--bsp", help="BSP to build.")

    args = parser.parse_args()
    apps = find_apps(
        args.paths,
        target=BSP2TARGET[args.bsp],
        #  recursive=True,
        build_dir=f"build_@n_@v_@w",
        config_rules=f"sdkconfig.bsp.{args.bsp}={args.bsp}",
        # build_log_filename="build_log.txt",
        size_json_filename="size.json",
    )
    ret_code = build_apps(apps, copy_sdkconfig=True)
    sys.exit(ret_code)
