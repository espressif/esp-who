set(COMPONENT_SRCS
    app_main.cpp
    app_dl.cpp
    )

set(COMPONENT_ADD_INCLUDEDIRS
    include
    )

set(COMPONENT_REQUIRES
    esp32-camera
    esp-dl
    common
    )

register_component()
