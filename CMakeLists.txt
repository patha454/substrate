cmake_minimum_required(VERSION 4.0)
project(substrate C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

install(TARGETS substrate
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/substrate
        DESTINATION include)

# --- Testing ---
option(BUILD_TESTS "Built Substrate's tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        cmocka
        GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git/
        GIT_TAG cmocka-1.1.8
        GIT_SHALLOW TRUE
    )
    # Disable Cmocka's extra targets and examples.
    set(WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(WITH_STATIC_LIB ON CACHE BOOL "" FORCE)
    set(UNIT_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(cmocka)
    enable_testing()
    add_executable(
        substrate_tests
            tests/test_dummy.c
    )
    target_link_libraries(substrate_tests
        PRIVATE
            substrate
            cmocka
    )
    add_test(NAME substrate_tests COMMAND substrate_tests)
endif()
