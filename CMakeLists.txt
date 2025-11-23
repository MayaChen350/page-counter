cmake_minimum_required(VERSION 4.0)
project(page_counter C)

set(CMAKE_C_STANDARD 99)

add_executable(page_counter main.c
        include/ttf.h
        page_counter.c
        page_counter.h
        include/ttf.c
        utils.c
        utils.h
)
