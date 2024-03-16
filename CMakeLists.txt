cmake_minimum_required(VERSION 3.27)
set(SUBSTRATE_PROJECT_NAME "substrate")
set(SUBSTRATE_PROJECT_DESCRIPTION "Freestanding, independent, static systems utilities")
set(SUBSTRATE_PROJECT_URL "http://example.com")

set(MAJOR_VERSION 0 CACHE STRING "Major version of the build")
set(MINOR_VERSION 0 CACHE STRING "Minor version of the build")
set(PATCH_VERSION 0 CACHE STRING "Patch version of the build")


project(
        ${SUBSTRATE_PROJECT_NAME}
        DESCRIPTION ${SUBSTRATE_PROJECT_DESCRIPTION}
        VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}"
        LANGUAGES C)

set(CMAKE_C_STANDARD 23)

add_library(substrate STATIC
        src/buffer.c)
target_include_directories(substrate PUBLIC include)
target_compile_options(${SUBSTRATE_PROJECT_NAME} PRIVATE "-Werror" "-pedantic" "-Wall" "-Wextra")
target_compile_options(${SUBSTRATE_PROJECT_NAME} PRIVATE "-ffreestanding")

add_subdirectory(test)
