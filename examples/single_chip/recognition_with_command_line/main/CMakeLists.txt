set(COMPONENT_SRCS 
    app_main.cpp
    app_facenet.c
    app_camera.cpp
    )

set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES
    esp32-camera
    esp-face
    )

register_component()
