set(include_dirs ".")

set(src_dirs ".")

set(requires esp_driver_mcpwm esp_driver_gpio)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})