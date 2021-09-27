cmake_minimum_required(VERSION 3.15.0)

project(cerberus VERSION 0.1.0)

option(USE_STATIC "Use static library?" ON)

message(STATUS "Compile static library? " ${USE_STATIC})

message(STATUS "CC " ${CMAKE_C_COMPILER})
message(STATUS "CXX " ${CMAKE_CXX_COMPILER})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cas -Wcast-align -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotio -Wformat=2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4 /Zc:__cplusplus")
endif ()

set(INCLUDE_DIRECTORIES ./include)

file(
        GLOB_RECURSE
        LIBRARY_OBJS
        "code/*.cpp" "code/*.c" "code/*.s"
)

file(
        GLOB_RECURSE
        EXAMPLE_OBJS
        "example/*.cpp" "example/*.s"
)

add_executable(
        example
        ${EXAMPLE_OBJS}
)

add_library(
        library-objs
        OBJECT
        ${LIBRARY_OBJS}
)

if (UNIX)
    set(EXTRA_FLAGS "-Wextra")
else ()
    set(EXTRA_FLAGS "")
endif ()

target_compile_options(
        library-objs
        PRIVATE
        -Wall ${EXTRA_FLAGS} -fno-stack-protector
        -nostdlib -ffreestanding -I ${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_options(
        example
        PRIVATE
        -Wall ${EXTRA_FLAGS} -I ${CMAKE_CURRENT_LIST_DIR}/include
)

if (USE_STATIC)
    add_library(
            ${PROJECT_NAME}
            STATIC
            $<TARGET_OBJECTS:library-objs>
    )

    target_link_options(
            example
            PRIVATE
            -static
    )

    target_link_libraries(
            example
            ${PROJECT_NAME}
    )
else ()
    target_compile_options(
            library-objs
            PRIVATE
            -fpic
    )

    add_library(
            ${PROJECT_NAME}
            SHARED
            $<TARGET_OBJECTS:library-objs>
    )

    target_link_libraries(
            example
            ${PROJECT_NAME}
    )

endif ()
