# target_add_aligned_binary_data adds binary data into the built target,
# by converting it to a generated source file which is then compiled
# to a binary object as part of the build
function(target_add_aligned_binary_data target embed_file embed_type)
    cmake_parse_arguments(_ "" "RENAME_TO" "DEPENDS" ${ARGN})
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(idf_path IDF_PATH)

    get_filename_component(embed_file "${embed_file}" ABSOLUTE)

    get_filename_component(name "${embed_file}" NAME)
    set(embed_srcfile "${build_dir}/${name}.S")

    set(rename_to_arg)
    if(__RENAME_TO)  # use a predefined variable name
        set(rename_to_arg -D "VARIABLE_BASENAME=${__RENAME_TO}")
    endif()

    add_custom_command(OUTPUT "${embed_srcfile}"
        COMMAND "${CMAKE_COMMAND}"
        -D "DATA_FILE=${embed_file}"
        -D "SOURCE_FILE=${embed_srcfile}"
        ${rename_to_arg}
        -D "FILE_TYPE=${embed_type}"
        -P "${COMPONENT_DIR}/cmake/data_file_embed_asm_aligned.cmake"
        MAIN_DEPENDENCY "${embed_file}"
        DEPENDS "${COMPONENT_DIR}/cmake/data_file_embed_asm_aligned.cmake" ${__DEPENDS}
        WORKING_DIRECTORY "${build_dir}"
        VERBATIM)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${embed_srcfile}")

    target_sources("${target}" PRIVATE "${embed_srcfile}")
endfunction()
