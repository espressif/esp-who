idf_component_register(SRC_DIRS . INCLUDE_DIRS include wallpaper REQUIRES esp32-camera esp-dl bus screen)

component_compile_options(-ffast-math -O3)
