set(srcs    app_main.cpp)

set(requires    esp-dl)

idf_component_register(SRCS ${srcs}
                       REQUIRES ${requires}
                       WHOLE_ARCHIVE)

if(CONFIG_MODEL_FILE_PATH)
    message("CONFIG_MODEL_FILE_PATH is defined")
endif()
message("CONFIG_MODEL_FILE_PATH: ${CONFIG_MODEL_FILE_PATH}")
message("PROJECT_DIR: ${PROJECT_DIR}")

set(MVMODEL_EXE ${PROJECT_DIR}/pack_model.py)
idf_build_get_property(build_dir BUILD_DIR)
set(image_file ${build_dir}/espdl_models/models.espdl)
set(MODEL_FILE_PATH ${PROJECT_DIR}/${CONFIG_MODEL_FILE_PATH})

add_custom_command(
    OUTPUT ${image_file}
    COMMENT "Move and Pack models..."
    COMMAND python ${MVMODEL_EXE} --model_path=${MODEL_FILE_PATH} --out_file=${image_file}
    DEPENDS ${MODEL_FILE_PATH}
    VERBATIM)

add_custom_target(models_espdl ALL DEPENDS ${image_file})
add_dependencies(flash models_espdl)

partition_table_get_partition_info(size "--partition-name model" "size")
partition_table_get_partition_info(offset "--partition-name model" "offset")

if("${size}" AND "${offset}")
    esptool_py_flash_to_partition(flash "model" "${image_file}")
else()
    set(message "Failed to find model in partition table file"
                "Please add a line(Name=model, Size>recommended size in log) to the partition file.")
endif()
