set(COMPONENT_SRCS "ov2640.c" "sccb.c" "ov3660.c" "sensor.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES soft_i2c)

register_component()
