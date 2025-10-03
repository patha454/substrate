cmake_minimum_required(VERSION 4.0)
project(substrate C)

set(CMAKE_C_STANDARD 23)

add_library(substrate STATIC
        include/substrate/memory/span.h
        src/memory/span.c)

target_include_directories(substrate
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Optional: Installation rules
install(TARGETS substrate
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/substrate
        DESTINATION include)