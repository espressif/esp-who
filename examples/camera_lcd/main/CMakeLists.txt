set(COMPONENT_SRCS "app_main.cpp" "app_dl.cpp")
# set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES
    esp32-camera
    esp-dl
    common
    bus
    screen)

register_component()
