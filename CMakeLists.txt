cmake_minimum_required(VERSION 3.31.6)
project(pagecounter C)

option(BUILD_WASM "Build the library for WebAssembly" OFF)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -ffunction-sections -fdata-sections")

if (BUILD_WASM)
    message(STATUS "Building for WebAssembly")
    set(CMAKE_C_FLAGS "-O3 -DNDEBUG -ffunction-sections -fdata-sections")
    add_executable(pagecounter_wasm
            utils.c
            include/ttf.c
            page_counter.c
    )
    set_target_properties(pagecounter_wasm PROPERTIES
            OUTPUT_NAME "pagecounter_lib"
            LINK_FLAGS "-sEXPORTED_FUNCTIONS=_getPageCount -sEXPORTED_RUNTIME_METHODS=ccall,cwrap"
    )
else ()
    # strip unused code apparently
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections")

    add_library(pagecounter_lib STATIC
            utils.c
            include/ttf.c
            page_counter.c
            ttf_reader.c
            ttf_reader.h
    )

    add_executable(pagecounter main.c)

    target_link_libraries(pagecounter PRIVATE pagecounter_lib m)
endif ()
