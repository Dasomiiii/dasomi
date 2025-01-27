cmake_minimum_required(VERSION 3.3)
project(Dasomi)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(Dasomi PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(Dasomi PRIVATE -Wno-variadic-macros -Wno-gnu-zero-variadic-macro-arguments)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Dasomi PRIVATE /W4 /O2)
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

add_executable(dasomi ${SOURCES})
