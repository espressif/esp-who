set(include_dirs ".")
set(src_dirs ".")
set (requires who_lcd who_task who_cam who_frame_capture)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})