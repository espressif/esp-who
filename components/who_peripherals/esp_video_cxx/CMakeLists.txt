set(include_dirs    .)
set(src_dirs        .)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES esp_timer)