idf_component_register(SRC_DIRS . INCLUDE_DIRS include REQUIRES esp32-camera esp-dl)

component_compile_options(-ffast-math -O3)
