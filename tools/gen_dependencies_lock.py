import os
from glob import glob
import subprocess
import shutil
import itertools

BSP2IDF_TARGET = {
    "esp32_s3_eye": "esp32s3",
    "esp32_s3_korvo_2": "esp32s3",
    "esp32_p4_function_ev_board": "esp32p4",
}
BSP2IDF_TARGET.update({k + "_noglib": v for k, v in BSP2IDF_TARGET.items()})

if __name__ == "__main__":
    root = "examples"
    project_dirs = [
        os.path.abspath(os.path.join(root, dir)) for dir in os.listdir(root)
    ]
    for project_dir in project_dirs:
        dep_locks = glob(os.path.join(project_dir, "dependencies.lock*"))
        for dep_lock in dep_locks:
            os.remove(dep_lock)

        build_dir = os.path.join(project_dir, "build")
        if os.path.isdir(build_dir):
            shutil.rmtree(build_dir)

        manifest = os.path.join(project_dir, "main/idf_component.yml")
        with open(manifest, "w", encoding="utf-8"):
            pass

        args_list = []
        sdkconfigs = glob(os.path.join(project_dir, "sdkconfig.bsp*"))
        if os.path.basename(project_dir) == "object_detect":
            detect_models = [
                "human_face_detect",
                "pedestrian_detect",
                "cat_detect",
                "dog_detect",
            ]
            for sdkconfig, detect_model in itertools.product(sdkconfigs, detect_models):
                args_list.append(
                    [
                        "idf.py",
                        f"-DSDKCONFIG_DEFAULTS={sdkconfig}",
                        f"-DDETECT_MODEL={detect_model}",
                        "set-target",
                        BSP2IDF_TARGET[sdkconfig.split(".")[-1]],
                    ]
                )
        else:
            for sdkconfig in sdkconfigs:
                args_list.append(
                    [
                        "idf.py",
                        f"-DSDKCONFIG_DEFAULTS={sdkconfig}",
                        "set-target",
                        BSP2IDF_TARGET[sdkconfig.split(".")[-1]],
                    ]
                )

        build_dir = os.path.join(project_dir, "build")
        for args in args_list:
            try:
                if os.path.isdir(build_dir):
                    shutil.rmtree(build_dir)
                subprocess.run(
                    args, cwd=project_dir, check=True, capture_output=True, text=True
                )
            except subprocess.CalledProcessError as e:
                print("Failed to set-target.")
                print("ret:", e.returncode)
                print("stdout:", e.stdout)
                print("stderr:", e.stderr)

        # clear manifest
        with open(manifest, "w", encoding="utf-8"):
            pass
