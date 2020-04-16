set(COMPONENT_SRCS
    app_main.c
    app_camera.c
    hd_uart.c
    )

set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    nvs_flash
    fb_gfx
    spiffs
    )

register_component()

