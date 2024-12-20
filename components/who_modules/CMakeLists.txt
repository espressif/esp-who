set(src_dirs        ./who_cam
                    ./who_lcd
                    ./who_app)

set(include_dirs    ./who_cam
                    ./who_lcd
                    ./who_app)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES human_face_recognition pedestrian_detect)