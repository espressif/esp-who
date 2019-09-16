set(COMPONENT_SRCS a.c)
set(COMPONENT_ADD_INCLUDEDIRS include)

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${COMPONENT_TARGET}
    esp_wakenet
    nn_model
    )
