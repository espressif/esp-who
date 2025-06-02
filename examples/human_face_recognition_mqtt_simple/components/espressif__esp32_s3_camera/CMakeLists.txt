idf_component_register(
    SRCS "esp32_s3_camera.c" "esp32_s3_camera_idf5.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES esp_driver_gpio esp_driver_sdmmc spiffs esp_driver_i2c fatfs
    PRIV_REQUIRES esp_lcd esp_driver_ledc esp_driver_spi
)
