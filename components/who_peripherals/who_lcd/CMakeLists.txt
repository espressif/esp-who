set(src_dirs        .)

set(include_dirs    .)

set (requires esp_lcd)
if (IDF_TARGET STREQUAL "esp32s3")
    list(APPEND requires esp32_s3_eye_noglib)
elseif (IDF_TARGET STREQUAL "esp32p4")
    list(APPEND requires esp32_p4_function_ev_board_noglib)
endif()

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

idf_build_get_property(build_components BUILD_COMPONENTS)
set(optional_reqs esp_lvgl_port espressif__esp_lvgl_port)
foreach(req ${optional_reqs})
    if(req IN_LIST build_components)
        idf_component_get_property(req_lib ${req} COMPONENT_LIB)
        add_library(who_lvgl_utils STATIC
            who_lvgl_utils/montserrat_bold_26.c
            who_lvgl_utils/montserrat_bold_20.c
            who_lvgl_utils/who_lvgl_utils.cpp
            )
        target_include_directories(who_lvgl_utils PUBLIC who_lvgl_utils)
        target_link_libraries(who_lvgl_utils PUBLIC ${req_lib})
        target_link_libraries(${COMPONENT_LIB} INTERFACE who_lvgl_utils)
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
        target_link_libraries(${bsp_lib} PUBLIC ${req_lib})
    endif()
endforeach()
