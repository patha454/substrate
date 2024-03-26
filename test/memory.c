

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include "memory.h"

static void test_sbMemoryValid_invalid_null(void ** state)
{
    (void) state;
    assert_false(sbMemoryValid(nullptr));
}

static void test_sbMemoryValid_invalid_nullptr(void ** state)
{
    (void) state;
    struct SbMemory memory = {
            .base = (intptr_t) (void*) nullptr,
            1
    };
    assert_false(sbMemoryValid(&memory));
}

static void test_sbMemoryValid_invalid_zero_len(void ** state)
{
    (void) state;
    uint8_t buffer[0];
    struct SbMemory memory = {
            .base = (intptr_t) & buffer,
            .length = 0
    };
    assert_false(sbMemoryValid(&memory));
}

static void test_sbMemoryValid_valid(void ** state)
{
    (void) state;
    const size_t length = 8;
    uint8_t buffer[length];
    struct SbMemory memory = {
            .base = (intptr_t) &buffer,
            .length = length
    };
    assert_true(sbMemoryValid(&memory));
}

static void test_sbMemory_configure(void ** state)
{
    (void) state;
    const size_t length = 8;
    uint8_t buffer[length];
    struct SbMemory memory = sbMemory(&buffer, length);
    assert_int_equal(memory.base, (intptr_t) &buffer);
    assert_int_equal(memory.length, length);
}

int main(void ) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_sbMemoryValid_invalid_null),
            cmocka_unit_test(test_sbMemoryValid_invalid_nullptr),
            cmocka_unit_test(test_sbMemoryValid_valid),
            cmocka_unit_test(test_sbMemoryValid_invalid_zero_len),
            cmocka_unit_test(test_sbMemory_configure)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
