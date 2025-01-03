set(src_dirs        ./)

set(include_dirs    ./)

set(requires    human_face_recognition
                spiflash_fatfs)

if (IDF_TARGET STREQUAL "esp32s3")
    list(APPEND requires esp32_s3_eye_noglib
                         esp_lcd)
elseif (IDF_TARGET STREQUAL "esp32p4")
    list(APPEND requires esp32_p4_function_ev_board_noglib
                         esp_lcd)
endif()

file(GLOB embed_files "${PROJECT_DIR}/faces/*.jpg")

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires} EMBED_FILES ${embed_files})
