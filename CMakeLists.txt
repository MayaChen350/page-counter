cmake_minimum_required(VERSION 4.0)
project(pagecounter C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER clang)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -ffunction-sections -fdata-sections")

# Enable Link‑Time Optimization (LTO)
# strip unused code apparently
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections")


add_library(pagecounter_lib STATIC
        page_counter.c
        include/ttf.c
        utils.c
)

add_executable(pagecounter main.c)

target_link_libraries(pagecounter PRIVATE pagecounter_lib m)