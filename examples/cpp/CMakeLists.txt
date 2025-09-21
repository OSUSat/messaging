cmake_minimum_required(VERSION 3.15)
project(osusat-example LANGUAGES CXX)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/osusat-build)

file(GLOB CPP_SOURCES "*.cpp" "src/*.cpp")

add_executable(my_cpp_project ${CPP_SOURCES})
target_link_libraries(my_cpp_project PRIVATE OSUSat::CPP)
