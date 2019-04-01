set(COMPONENT_SRCS
    app_camera.c
    app_facenet.c
    app_httpserver.c
    app_main.c
    app_wifi.c
    )

set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    esp_http_server
    fb_gfx
    network_blufi
    )

register_component()
