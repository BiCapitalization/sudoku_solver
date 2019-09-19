cmake_minimum_required(VERSION 3.12)
project(sudoku_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ADDITIONAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)
set(GNU_CLANG_WARNING_FLAGS -Wall -Wextra -Wpedantic)
set(GNU_CLANG_OPTIMIZATION_FLAGS
    -march=native -mtune=native -fno-exceptions -fno-rtti)

option(BuildTests "Build the test suite" OFF)

include(FetchContent)

include(cmake/FetchExpected.cmake)
include(cmake/FetchFmt.cmake)

if(BuildTests)
    include(cmake/FetchCatch2.cmake)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if(BuildTests)
    add_subdirectory(test)
endif()
