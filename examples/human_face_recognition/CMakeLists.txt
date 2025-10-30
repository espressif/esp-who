# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ../../components/who_task
                         ../../components/who_peripherals/who_usb
                         ../../components/who_peripherals/who_cam
                         ../../components/who_peripherals/who_lcd
                         ../../components/who_peripherals/who_spiflash_fatfs
                         ../../components/who_frame_cap
                         ../../components/who_frame_lcd_disp
                         ../../components/who_detect
                         ../../components/who_recognition
                         ../../components/who_app/who_recognition_app)

add_compile_options(-fdiagnostics-color=always)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
if (NOT DEFINED BSP)
    message(FATAL_ERROR "BSP is not defined, please make sure that the environment variable IDF_EXTRA_ACTIONS_PATH is properly set.")
endif()
idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${BSP})
project(human_face_recognition)
