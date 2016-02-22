cmake_minimum_required(VERSION 2.8)
project(sdl2_maprgb_test)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_definitions(-std=c99)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

add_executable(sdl2_maprgb_test main.c)

target_link_libraries(sdl2_maprgb_test ${SDL2_LIBRARY} ${SDLMAIN_LIBRARY})
