set(COMPONENT_SRCS
    scanner/config.c
    scanner/decoder.c
    scanner/error.c
    scanner/image.c
    scanner/img_scanner.c
    scanner/refcnt.c
    scanner/scanner.c
    scanner/symbol.c
    #decoder/codabar.c
    decoder/code39.c
    #decoder/code93.c
    decoder/code128.c
    #decoder/databar.c
    #decoder/ean.c
    #decoder/i25.c
    decoder/qr_finder.c
    qrcode/bch15_5.c
    qrcode/binarize.c
    qrcode/isaac.c
    qrcode/qrdec.c
    qrcode/qrdectxt.c
    qrcode/rs.c
    qrcode/util.c
)

if (ENABLE_PDF417)
    set( COMPONENT_SRCS += decoder/pdf417.c )
endif (ENABLE_PDF417)

if (ENABLE_SQCODE)
    set( COMPONENT_SRCS += scanner/sqcode.c 
                           decoder/sq_finder.c )
endif (ENABLE_SQCODE)


set(COMPONENT_REQUIRES
    newlib
    heap
    )

set(COMPONENT_ADD_INCLUDEDIRS
    scanner/include
    decoder/include
    qrcode/include
    lib/include
)

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET}
    ccs
    ces
    conv
    newlib_c
)
