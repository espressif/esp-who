idf_component_register(
    SRCS "esp32_s3_korvo_2.c" "esp32_s3_korvo_2_idf5.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES esp_driver_gpio esp_driver_i2s esp_driver_sdmmc esp_driver_i2c esp_adc fatfs
    PRIV_REQUIRES spiffs esp_lcd esp_driver_spi esp_driver_ledc
)
