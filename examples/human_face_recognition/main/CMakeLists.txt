set(src_dirs        ./)

set(include_dirs    ./)

set(requires esp-dl
             human_face_detect
             human_face_recognition
             spiflash_fatfs
             who_modules)

if (IDF_TARGET STREQUAL "esp32s3")
    list(APPEND requires esp32_s3_eye)
elseif (IDF_TARGET STREQUAL "esp32p4")
    list(APPEND requires esp_video
                         esp32_p4_function_ev_board)
endif()

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})




