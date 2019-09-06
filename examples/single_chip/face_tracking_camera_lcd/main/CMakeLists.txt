set(COMPONENT_SRCS
    app_main.cpp
    app_facenet.cpp
    app_camera.cpp
    app_lcd.cpp
    )

set(COMPONENT_ADD_INCLUDEDIRS
    include
    )

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    lcd
    fb_gfx
    )

register_component()
