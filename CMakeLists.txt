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

# Tests
add_executable(test_multiplot tests/multiplot.c)
target_link_libraries(test_multiplot PRIVATE tycho)

add_executable(test_simple_2d tests/simple_2d.c)
target_link_libraries(test_simple_2d PRIVATE tycho)

add_executable(test_simple_3d tests/simple_3d.c)
target_link_libraries(test_simple_3d PRIVATE tycho)

if(NOT WIN32)
    target_link_libraries(test_multiplot PRIVATE m)
    target_link_libraries(test_simple_2d PRIVATE m)
    target_link_libraries(test_simple_3d PRIVATE m)
endif()

if(NOT WIN32)
    target_link_libraries(test_multiplot PRIVATE m)
endif()