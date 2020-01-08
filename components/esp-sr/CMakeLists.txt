set(COMPONENT_SRCS dummy.c)
set(COMPONENT_ADD_INCLUDEDIRS 
    lib/include
    wake_word_engine/include
    acoustic_algorithm/include
    )


register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/wake_word_engine")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/acoustic_algorithm")
target_link_libraries(${COMPONENT_TARGET} 
    dl_lib_sr
    c_speech_features
    wakenet
    hilexin_wn5
    esp_audio_processor
    )