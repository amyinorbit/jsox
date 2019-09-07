cmake_minimum_required(VERSION 3.12)
project(JSOx VERSION 2019.9 LANGUAGES C)

enable_testing()

include_directories(PUBLIC include)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_GENERATOR STREQUAL "Ninja")
    set(CMAKE_C_FLAGS "-fcolor-diagnostics ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(packages/UniC)

add_subdirectory(src)
# add_subdirectory(tests)
