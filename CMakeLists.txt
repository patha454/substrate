cmake_minimum_required(VERSION 4.0)
project(substrate C)

set(CMAKE_C_STANDARD 23)

add_library(substrate STATIC
        memory/span.h)
