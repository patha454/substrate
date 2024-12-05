

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include "memory.h"

static void test_sbMemoryValid_invalid_nullptr(void ** state)
{
    (void) state;
    const struct _SbMemory memory = {
            .base = (intptr_t) (void*) nullptr,
            1
    };
    assert_false(sbMemoryValid(memory));
}

static void test_sbMemoryValid_invalid_zero_len(void ** state)
{
    (void) state;
    uint8_t buffer[0];
    const struct _SbMemory memory = {
            .base = (intptr_t) &buffer,
            .length = 0
    };
    assert_false(sbMemoryValid(memory));
}

static void test_sbMemoryValid_valid(void ** state)
{
    (void) state;
    constexpr size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = {
            .base = (intptr_t) &buffer,
            .length = length
    };
    assert_true(sbMemoryValid(memory));
}

static void test_sbMemory_configure(void ** state)
{
    (void) state;
    constexpr size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = sbMemory(&buffer, length);
    assert_int_equal(memory.base, (intptr_t) &buffer);
    assert_int_equal(memory.length, length);
}

static void test_sbMemoryOffset_zero(void ** state)
{
    (void) state;
    constexpr size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = {
            .base = (intptr_t ) &buffer,
            .length = length
    };
    assert_int_equal(sbMemoryOffset(memory, 0), &buffer);
}

static void test_sbMemoryOffset_positive(void ** state)
{
    (void) state;
    constexpr size_t length = 8;
    size_t offset = 4;
    uint8_t buffer[length];
    const struct _SbMemory memory = {
            .base = (intptr_t ) &buffer,
            .length = length
    };
    assert_int_equal(sbMemoryOffset(memory, offset), &buffer[offset]);
}

static void test_sbMemoryOffset_max(void ** state)
{
    (void) state;
    constexpr size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = {
            .base = (intptr_t ) &buffer,
            .length = length
    };
    assert_int_equal(sbMemoryOffset(memory, length - 1), &buffer[length - 1]);
}

static void test_sbMemoryOffset_out_of_range(void ** state)
{
    (void) state;
    size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = {
            .base = (intptr_t ) &buffer,
            .length = length
    };
    assert_null(sbMemoryOffset(memory, length));
}

static void test_sbMemoryOffset_invalid(void ** state)
{
    (void) state;
    const struct _SbMemory memory = {
            .base = (intptr_t ) (void*) nullptr,
            .length = 0
    };
    assert_null(sbMemoryOffset(memory, 0));
}

int main(void ) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_sbMemoryValid_invalid_nullptr),
            cmocka_unit_test(test_sbMemoryValid_valid),
            cmocka_unit_test(test_sbMemoryValid_invalid_zero_len),
            cmocka_unit_test(test_sbMemory_configure),
            cmocka_unit_test(test_sbMemoryOffset_zero),
            cmocka_unit_test(test_sbMemoryOffset_out_of_range),
            cmocka_unit_test(test_sbMemoryOffset_invalid),
            cmocka_unit_test(test_sbMemoryOffset_positive),
            cmocka_unit_test(test_sbMemoryOffset_max)
    };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
