set(src_dirs        ./cam
                    ./lcd)

set(include_dirs    ./cam
                    ./lcd)


set (requires esp_lcd esp_timer esp-dl)
if (IDF_TARGET STREQUAL "esp32s3")
    list(APPEND requires esp32_s3_eye_noglib)
elseif (IDF_TARGET STREQUAL "esp32p4")
    list(APPEND requires esp32_p4_function_ev_board_noglib
                         esp_video)
endif()

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

idf_build_get_property(build_components BUILD_COMPONENTS)
set(optional_reqs esp_lvgl_port espressif__esp_lvgl_port)
foreach(req ${optional_reqs})
    if(req IN_LIST build_components)
        idf_component_get_property(req_lib ${req} COMPONENT_LIB)
        if (IDF_TARGET STREQUAL "esp32s3")
            if (esp32_s3_eye_noglib IN_LIST build_components)
                idf_component_get_property(bsp_lib esp32_s3_eye_noglib COMPONENT_LIB)
            elseif(espressif__esp32_s3_eye_noglib IN_LIST build_components)
                idf_component_get_property(bsp_lib espressif__esp32_s3_eye_noglib COMPONENT_LIB)
            endif()
        elseif (IDF_TARGET STREQUAL "esp32p4")
            if (esp32_p4_function_ev_board_noglib IN_LIST build_components)
                idf_component_get_property(bsp_lib esp32_p4_function_ev_board_noglib COMPONENT_LIB)
            elseif(espressif__esp32_p4_function_ev_board_noglib IN_LIST build_components)
                idf_component_get_property(bsp_lib espressif__esp32_p4_function_ev_board_noglib COMPONENT_LIB)
            endif()
        endif()
        target_compile_definitions(${bsp_lib} PUBLIC "BSP_CONFIG_NO_GRAPHIC_LIB=0")
        message(${bsp_lib} ${req_lib})
        target_link_libraries(${bsp_lib} PUBLIC ${req_lib})
    endif()
endforeach()
