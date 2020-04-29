set(COMPONENT_SRCS
    app_main.c
    )

set(COMPONENT_ADD_INCLUDEDIRS
    ../../output
    ../../../lib/include
    )

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib")
target_link_libraries(${COMPONENT_TARGET}
    dl_lib
    )
