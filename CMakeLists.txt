cmake_minimum_required(VERSION 3.15)

project(osusat-generator
    VERSION 1.0.0
    DESCRIPTION "OSUSat/SCRT messaging standard"
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0135 NEW)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall -Wextra -Wpedantic)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/c/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
)

find_program(CLANG_FORMAT_EXECUTABLE clang-format)
find_program(CLANG_TIDY_EXECUTABLE clang-tidy)

file(GLOB_RECURSE C_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/c/src/*.c"
)

file(GLOB_RECURSE CXX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/*.cpp"
)

file(GLOB_RECURSE C_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/c/include/*.h"
)

file(GLOB_RECURSE CXX_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/include/*.hxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/include/*.h"
)

if(C_SOURCES)
    add_library(osusat-c ${C_SOURCES})
    target_include_directories(osusat-c PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/c/include
    )
    set_target_properties(osusat-c PROPERTIES C_STANDARD 11)
else()
    add_library(osusat-c INTERFACE)
    target_include_directories(osusat-c INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/c/include
    )
endif()

add_library(OSUSat::C ALIAS osusat-c)

if(CXX_SOURCES)
    add_library(osusat-cpp ${CXX_SOURCES})
    target_include_directories(osusat-cpp PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
    )
    set_target_properties(osusat-cpp PROPERTIES CXX_STANDARD 14)
else()
    add_library(osusat-cpp INTERFACE)
    target_include_directories(osusat-cpp INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
    )
endif()

add_library(OSUSat::CPP ALIAS osusat-cpp)

if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${C_SOURCES} ${CXX_SOURCES} ${C_HEADERS} ${CXX_HEADERS}
        COMMENT "Formatting source code with clang-format"
        VERBATIM
    )
endif()

if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(lint
        COMMAND ${CLANG_TIDY_EXECUTABLE} ${C_SOURCES} ${CXX_SOURCES} -- 
            -I${CMAKE_CURRENT_SOURCE_DIR}/c/include 
            -I${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
        COMMENT "Running clang-tidy linter"
        VERBATIM
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "=== OSUSat/SCRT Messaging Standard Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

if(CLANG_FORMAT_EXECUTABLE)
    message(STATUS "clang-format: ${CLANG_FORMAT_EXECUTABLE}")
endif()

if(CLANG_TIDY_EXECUTABLE)
    message(STATUS "clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
endif()

message(STATUS "============================================")

if(C_SOURCES)
    install(TARGETS osusat-c
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

if(CXX_SOURCES)
    install(TARGETS osusat-cpp
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()


install(DIRECTORY c/include/ DESTINATION include/osusat-c
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY cpp/include/ DESTINATION include/osusat-cpp
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.hxx" PATTERN "*.h"
)
