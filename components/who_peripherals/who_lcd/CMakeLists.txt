set(src_dirs        .)

set(include_dirs    .)

set(requires esp_lcd)

set(bsp_components )

set(bsp_components esp32_s3_eye_noglib espressif__esp32_s3_eye_noglib
                   esp32_s3_eye espressif__esp32_s3_eye
                   esp32_s3_korvo_2 espressif__esp32_s3_korvo_2
                   esp32_s3_korvo_2_noglib espressif__esp32_s3_korvo_2_noglib
                   esp32_p4_function_ev_board_noglib espressif__esp32_p4_function_ev_board_noglib
                   esp32_p4_function_ev_board espressif__esp32_p4_function_ev_board)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
idf_component_optional_requires(PUBLIC ${bsp_components})

idf_build_get_property(build_components BUILD_COMPONENTS)
set(lvgl_port_components esp_lvgl_port espressif__esp_lvgl_port)
foreach(lvgl_port_component ${lvgl_port_components})
    if(lvgl_port_component IN_LIST build_components)
        idf_component_get_property(lvgl_port_lib ${lvgl_port_component} COMPONENT_LIB)
        add_library(who_lvgl_utils STATIC
            who_lvgl_utils/montserrat_bold_26.c
            who_lvgl_utils/montserrat_bold_20.c
            who_lvgl_utils/who_lvgl_utils.cpp
            )
        target_include_directories(who_lvgl_utils PUBLIC who_lvgl_utils)
        target_link_libraries(who_lvgl_utils PUBLIC ${lvgl_port_lib})
        target_link_libraries(${COMPONENT_LIB} INTERFACE who_lvgl_utils)
    endif()
endforeach()
