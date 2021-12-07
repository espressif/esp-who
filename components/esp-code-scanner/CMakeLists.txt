idf_build_get_property(target IDF_TARGET)

if(${IDF_TARGET} STREQUAL "esp32")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32" "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")

elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s2" "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")

elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3" "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")

endif()

set(COMPONENT_ADD_INCLUDEDIRS   include)

register_component()

target_link_libraries(${COMPONENT_TARGET}   INTERFACE   ${links})

set(lib     libesp-code-scanner.a
            libnewlib_iconv.a)
target_link_libraries(${COMPONENT_TARGET}   INTERFACE   ${lib})