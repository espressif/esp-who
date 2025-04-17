set(src_dirs        .)

set(include_dirs    .)

set(requires who_task who_cam who_lcd)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})