cmake_minimum_required(VERSION 3.31.6)
project(pagecounter C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_RELEASE "-O3 -ffunction-sections -fdata-sections")

# strip unused code apparently
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections")
add_library(pagecounter_lib STATIC
            utils.c
            include/ttf.c
            page_counter.c
)

add_executable(pagecounter main.c)

target_link_libraries(pagecounter PRIVATE pagecounter_lib)
