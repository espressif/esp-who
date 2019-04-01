set(COMPONENT_SRCS
    wechat_blufi.c
    wechat_security.c
    )

set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES
    bt
    nvs_flash
    mdns
    )

register_component()
