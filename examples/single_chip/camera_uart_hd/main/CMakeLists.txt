set(COMPONENT_SRCS "app_main.c" "hd_uart.c" "app_camera.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    nvs_flash
    fb_gfx
    mdns
    )

register_component()
