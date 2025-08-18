add_library(VolvoLib)

target_sources(VolvoLib
    PRIVATE
        src/d2/d2_message.cpp
)

target_include_directories(VolvoLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
