set(COMPONENT_SRCS
    main.c
    image_util.c
    lssh_forward.c
    pe_forward.c
    fb_gfx.c
    )

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${COMPONENT_TARGET}
    fd_tie
    pe_tie
    lssh_tie
    dl_tie
    )

