include(FetchContent)
include(CTest)

FetchContent_Declare(cmocka
        GIT_REPOSITORY "https://git.cryptomilk.org/projects/cmocka.git"
        GIT_TAG "stable-1.1"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(cmocka)


add_executable(buffer_tests_bin test_buffer.c)
target_link_libraries(buffer_tests_bin PRIVATE cmocka substrate)
add_test(NAME buffer_tests COMMAND buffer_tests_bin)