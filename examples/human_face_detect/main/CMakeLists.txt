set(src_dirs        ./)

set(include_dirs    ./)

set(requires esp-dl
             human_face_detect
             esp_video
             esp32_p4_function_ev_board)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})




