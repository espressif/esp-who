set(COMPONENT_SRCS
    app_main.c
    app_camera.c
    app_httpserver.c
    app_speech_recsrc.c
    app_speech_wakeup.c
    app_wifi.c
    )

set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    nvs_flash
    esp_http_server
    fb_gfx
    esp-sr
    spiffs
    )

register_component()

