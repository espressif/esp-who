set(include_dirs    .
                    who_uvc_cam)
set(src_dirs who_uvc_cam)

set(requires esp_timer esp-dl esp_lcd who_usb usb_host_uvc)

set(bsp_components esp32_s3_eye espressif__esp32_s3_eye
                   esp32_s3_eye_noglib espressif__esp32_s3_eye_noglib
                   esp32_s3_korvo_2 espressif__esp32_s3_korvo_2
                   esp32_s3_korvo_2_noglib espressif__esp32_s3_korvo_2_noglib
                   esp32_p4_function_ev_board espressif__esp32_p4_function_ev_board
                   esp32_p4_function_ev_board_noglib espressif__esp32_p4_function_ev_board_noglib)

if (IDF_TARGET STREQUAL "esp32s3")
    list(APPEND src_dirs who_s3_cam)
    list(APPEND include_dirs who_s3_cam)
elseif (IDF_TARGET STREQUAL "esp32p4")
    list(APPEND src_dirs who_p4_cam)
    list(APPEND include_dirs who_p4_cam)
    list(APPEND requires esp_video)
endif()

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
idf_component_optional_requires(PUBLIC ${bsp_components})
