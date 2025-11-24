cmake_minimum_required(VERSION 4.0)
project(pagecounter C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -ffunction-sections -fdata-sections")

# strip unused code apparently
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections")

# Fix the "archive has no index" problem
set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

add_library(pagecounter_lib STATIC
        utils.c
        include/ttf.c
        page_counter.c
)

add_executable(pagecounter main.c)

target_link_libraries(pagecounter PRIVATE pagecounter_lib m)