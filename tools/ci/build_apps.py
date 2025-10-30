from idf_build_apps import build_apps, find_apps
import sys
import os

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
    example_dir = os.environ.get("EXAMPLE_DIR")
    sdkconfig_defaults = os.environ.get("SDKCONFIG_DEFAULTS")
    bsp = sdkconfig_defaults.split(".")[-1]

    apps = find_apps(
        example_dir,
        target=BSP2TARGET[bsp],
        #  recursive=True,
        build_dir=f"build_@n_@v_@w",
        config_rules=f"{sdkconfig_defaults}={bsp}",
        # build_log_filename="build_log.txt",
        size_json_filename="size.json",
    )
    ret_code = build_apps(apps, copy_sdkconfig=True)
    sys.exit(ret_code)
