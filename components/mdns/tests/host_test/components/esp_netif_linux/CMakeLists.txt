idf_component_register(SRCS esp_netif_linux.c
                       INCLUDE_DIRS include $ENV{IDF_PATH}/components/esp_netif/include
                       REQUIRES esp_event)
