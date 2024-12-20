#IDF version is less than IDF5.0
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_LESS "5.0")
    set(SRC_VER "esp32_s3_eye_idf4.c")
else()
    set(SRC_VER "esp32_s3_eye_idf5.c")
endif()

idf_component_register(
    SRCS "esp32_s3_eye.c" ${SRC_VER}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver spiffs
    PRIV_REQUIRES fatfs esp_lcd
)
