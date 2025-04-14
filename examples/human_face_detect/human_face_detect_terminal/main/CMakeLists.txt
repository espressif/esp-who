set(src_dirs        ./)

set(include_dirs    ./)

set(requires who_cam
             who_lcd
             who_frame_capture
             who_detect
             who_detect_app
             human_face_detect)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
