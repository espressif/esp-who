if(CONFIG_MDNS_NETWORKING_SOCKET)
    set(MDNS_NETWORKING "mdns_networking_socket.c")
else()
    set(MDNS_NETWORKING "mdns_networking_lwip.c")
endif()

idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    set(dependencies esp_event esp_netif_linux esp_timer_linux esp_system)
    set(srcs "mdns.c" ${MDNS_NETWORKING})
else()
    set(dependencies lwip console esp_netif)
    set(private_dependencies esp_timer esp_wifi)
    set(srcs "mdns.c" ${MDNS_NETWORKING} "mdns_console.c")
endif()

idf_component_register(
        SRCS ${srcs}
        INCLUDE_DIRS "include"
        PRIV_INCLUDE_DIRS "private_include"
        REQUIRES ${dependencies}
        PRIV_REQUIRES ${private_dependencies})
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(${target} STREQUAL "linux")
    target_link_libraries(${COMPONENT_LIB} PRIVATE "-lbsd")
endif()


if(CONFIG_ETH_ENABLED)
    idf_component_optional_requires(PRIVATE esp_eth)
endif()
