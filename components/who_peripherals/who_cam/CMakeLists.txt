set(include_dirs    .)

set (requires esp_timer esp-dl esp_lcd)
if (IDF_TARGET STREQUAL "esp32s3")
    set(src_dirs who_cam_s3)
    list(APPEND include_dirs who_cam_s3)
    list(APPEND requires esp32_s3_eye_noglib)
elseif (IDF_TARGET STREQUAL "esp32p4")
    set(src_dirs who_cam_p4)
    list(APPEND include_dirs who_cam_p4)
    list(APPEND requires esp_video
                         esp32_p4_function_ev_board_noglib)
endif()

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
