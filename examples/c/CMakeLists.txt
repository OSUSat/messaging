cmake_minimum_required(VERSION 3.15)
project(osusat-example LANGUAGES C)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/osusat-build)

file(GLOB C_SOURCES "*.c" "src/*.c")

add_executable(my_c_project ${C_SOURCES})
target_link_libraries(my_c_project PRIVATE OSUSat::C)
