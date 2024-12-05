include(FetchContent)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)

FetchContent_Declare(cmocka
        GIT_REPOSITORY "https://git.cryptomilk.org/projects/cmocka.git"
        GIT_TAG "stable-1.1"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)
FetchContent_MakeAvailable(cmocka)


add_executable(memory_tests_bin memory.c)
target_link_libraries(memory_tests_bin PRIVATE cmocka substrate)
add_test(NAME memory_tests COMMAND memory_tests_bin)