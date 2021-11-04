cmake_minimum_required(VERSION 3.15.0)

project(cerberus VERSION 0.1.0)

option(USE_STATIC "Use static library?" FALSE)

message(STATUS "Compile static library? " ${USE_STATIC})

message(STATUS "CC " ${CMAKE_C_COMPILER})
message(STATUS "CXX " ${CMAKE_CXX_COMPILER})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wshadow -Wnon-virtual-dtor \
        -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion \
        -Wnull-dereference -Wdouble-promotion -Wformat=2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4 /Zc:__cplusplus")
endif ()

set(INCLUDE_DIRECTORIES ./include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

file(
        GLOB_RECURSE
        EXAMPLE_OBJS
        "example/main.cpp"
)

file(
        GLOB_RECURSE
        CALCULATOR_OBJS
        "example/calculator.cpp"
)

file(
        GLOB_RECURSE
        CERB2LEX_OBJS
        "programs/cerb2lex.cpp"
)

file(
        GLOB_RECURSE
        CALCULATOR_OBJS
        "example/calculator.cpp"
)

add_executable(
        example
        ${EXAMPLE_OBJS}
)

add_executable(
        cerb2lex
        ${CERB2LEX_OBJS}
)

add_executable(
        calculator
        ${CALCULATOR_OBJS}
)

find_package(fmt CONFIG REQUIRED)


target_link_libraries(
        example
        fmt::fmt
)

target_link_libraries(
        cerb2lex
        fmt::fmt
)

target_link_libraries(
        calculator
        fmt::fmt
)
