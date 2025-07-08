set(src_dirs        .
                    ../who_app_common
                    ../who_app_common/who_detect_result_handle)

set(include_dirs    .
                    ../who_app_common
                    ../who_app_common/who_detect_result_handle)

set(requires who_detect who_frame_lcd_disp)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})