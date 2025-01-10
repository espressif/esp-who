set(src_dirs        ./)

set(include_dirs    ./)

set(requires who_cam_lcd
             who_recognition
             spiflash_fatfs)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})




