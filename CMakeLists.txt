cmake_minimum_required(VERSION 3.15)
project(tycho C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(tycho STATIC
    src/plotter.c
)

target_include_directories(tycho PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)