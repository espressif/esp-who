set(src_dirs        .
                    src)

set(include_dirs    include
                    .)

set(requires    console
                esp_adc_cal
                        )

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs})