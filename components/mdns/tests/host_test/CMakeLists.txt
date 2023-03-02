cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "../..")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main esp_netif_linux)
project(mdns_host)
