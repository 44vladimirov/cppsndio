cmake_minimum_required(VERSION 3.18)
project(cppsndio)

find_library(SNDIO_LIB sndio REQUIRED)
find_path(SNDIO_INCLUDE sndio.h REQUIRED)

add_library(${PROJECT_NAME} OBJECT)

target_sources(${PROJECT_NAME}
    PRIVATE
        SndioPlayback.cpp
        SndioPlayback.hpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        .
        ${SNDIO_INCLUDE}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SNDIO_LIB}
)
