idf_component_register(SRCS esp_timer_linux.c timer_task.cpp
                       INCLUDE_DIRS include
                       REQUIRES esp_event)

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
