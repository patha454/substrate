#include "memory.h"

// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

static void test_sbMemoryValid_invalidNullPtr(void** state)
{
    (void)state;
    const struct _SbMemory memory = { .base = (intptr_t)(void*)nullptr, 1 };
    assert_false(sbMemoryValid(memory));
}

static void test_sbMemoryValid_invalidZeroLen(void** state)
{
    (void)state;
    uint8_t buffer[0];
    const struct _SbMemory memory = { .base = (intptr_t)&buffer, .length = 0 };
    assert_false(sbMemoryValid(memory));
}

static void test_sbMemoryValid_valid(void** state)
{
    (void)state;
    const size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory
        = { .base = (intptr_t)&buffer, .length = length };
    assert_true(sbMemoryValid(memory));
}

static void test_sbMemory_configure(void** state)
{
    (void)state;
    const size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory = sbMemory(&buffer, length);
    assert_int_equal(memory.base, (intptr_t)&buffer);
    assert_int_equal(memory.length, length);
}

static void test_sbMemoryOffset_zero(void** state)
{
    (void)state;
    const size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory
        = { .base = (intptr_t)&buffer, .length = length };
    assert_int_equal(sbMemoryOffset(memory, 0), &buffer);
}

static void test_sbMemoryOffset_positive(void** state)
{
    (void)state;
    const size_t length = 8;
    size_t offset = 4;
    uint8_t buffer[length];
    const struct _SbMemory memory
        = { .base = (intptr_t)&buffer, .length = length };
    assert_int_equal(sbMemoryOffset(memory, offset), &buffer[offset]);
}

static void test_sbMemoryOffset_max(void** state)
{
    (void)state;
    const size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory
        = { .base = (intptr_t)&buffer, .length = length };
    assert_int_equal(sbMemoryOffset(memory, length - 1), &buffer[length - 1]);
}

static void test_sbMemoryOffset_outOfRange(void** state)
{
    (void)state;
    size_t length = 8;
    uint8_t buffer[length];
    const struct _SbMemory memory
        = { .base = (intptr_t)&buffer, .length = length };
    assert_null(sbMemoryOffset(memory, length));
}

static void test_sbMemoryOffset_invalid(void** state)
{
    (void)state;
    const struct _SbMemory memory
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    assert_null(sbMemoryOffset(memory, 0));
}

static void test_sbMemoryOverlapLow_invalidReference(void** state)
{
    (void)state;
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t query_bytes = { 0 };
    const struct _SbMemory query = sbMemory(&query_bytes, sizeof(query_bytes));
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_invalidQuery(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t reference_bytes = { 0 };
    const struct _SbMemory reference
        = sbMemory(&reference_bytes, sizeof(reference_bytes));
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_invalidReferenceAndQuery(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_simple(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 4);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_equal(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 4);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_equalBase(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_equalFull(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_superOverlap(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_noOverlapLow(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[4], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_noOverlapHigh(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[4], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_noOverlapTouchLow(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_noOverlapHighTouch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_overlapInside(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 8);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapHigh_invalidReference(void** state)
{
    (void)state;
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t query_bytes = { 0 };
    const struct _SbMemory query = sbMemory(&query_bytes, sizeof(query_bytes));
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_invalidQuery(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t reference_bytes = { 0 };
    const struct _SbMemory reference
        = sbMemory(&reference_bytes, sizeof(reference_bytes));
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_invalidReferenceAndQuery(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_simple(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes[2], 4);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_equal(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 4);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_equalBase(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 6);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_equalEnd(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 4);
    const sb_memory_t query = sbMemory(&bytes, 6);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_superOverlap(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_noOverlapLow(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[4], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_noOverlapHigh(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[4], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_noOverlapLowTouch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_noOverlapHighTouch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_overlapInside(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 8);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryCopy_zeroLenOrigin(void** state)
{
    (void)state;
    size_t length = 1;
    const uint8_t origin[] = { 1 };
    const uint8_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, 0);
    const sb_memory_t destination_mem = sbMemory(&destination, length);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_zeroLenDestination(void** state)
{
    (void)state;
    size_t length = 1;
    const uint8_t origin[] = { 1 };
    const uint8_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, length);
    const sb_memory_t destination_mem = sbMemory(&destination, 0);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_noOverlapSingleByte(void** state)
{
    (void)state;
    size_t length = 1;
    const uint8_t origin[] = { 1 };
    const uint8_t destination[] = { 0 };
    const sb_memory_t origin_mem = sbMemory(&origin, length);
    const sb_memory_t destination_mem = sbMemory(&destination, length);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&origin, &destination, length);
}

static void test_sbMemoryCopy_noOverlapEightByte(void** state)
{
    (void)state;
    size_t length = 8;
    const uint8_t origin[] = { 8, 7, 6, 5, 4, 3, 2, 1 };
    const uint8_t destination[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const sb_memory_t origin_mem = sbMemory(&origin, length);
    const sb_memory_t destination_mem = sbMemory(&destination, length);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&origin, &destination, length);
}

static void test_sbMemoryCopy_forwardOverlap(void** state)
{
    (void)state;
    const uint8_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer, 4);
    const sb_memory_t destination_mem = sbMemory(&buffer[2], 4);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint8_t[]) { 1, 2, 1, 2, 3, 4, 7, 8 }), 8);
}

static void test_sbMemoryCopy_backwardOverlap(void** state)
{
    (void)state;
    const uint8_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer[2], 4);
    const sb_memory_t destination_mem = sbMemory(&buffer, 4);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint8_t[]) { 3, 4, 5, 6, 5, 6, 7, 8 }), 8);
}

static void test_sbMemoryCopy_wordszeroLenOrigin(void** state)
{
    (void)state;
    const uint64_t origin[] = { 1 };
    const uint64_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, 0);
    const sb_memory_t destination_mem = sbMemory(&destination, 1);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_wordsZeroLenDestination(void** state)
{
    (void)state;
    const uint64_t origin[] = { 1 };
    const uint64_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, 1);
    const sb_memory_t destination_mem = sbMemory(&destination, 0);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_wordsNoOverlapSingleWord(void** state)
{
    (void)state;
    const uint64_t origin[] = { 1 };
    const uint64_t destination[] = { 0 };
    const sb_memory_t origin_mem = sbMemory(&origin, sizeof(origin));
    const sb_memory_t destination_mem
        = sbMemory(&destination, sizeof(destination));
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&origin, &destination, sizeof(origin));
}

static void test_sbMemoryCopy_wordsNoOverlapEightWords(void** state)
{
    (void)state;
    const uint64_t origin[] = { 8, 7, 6, 5, 4, 3, 2, 1 };
    const uint64_t destination[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const sb_memory_t origin_mem = sbMemory(&origin, sizeof(origin));
    const sb_memory_t destination_mem
        = sbMemory(&destination, sizeof(destination));
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&origin, &destination, sizeof(origin));
}

static void test_sbMemoryCopy_wordsForwardOverlap(void** state)
{
    (void)state;
    const uint64_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer, 4 * sizeof(buffer[0]));
    const sb_memory_t destination_mem
        = sbMemory(&buffer[2], 4 * sizeof(buffer[0]));
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint64_t[]) { 1, 2, 1, 2, 3, 4, 7, 8 }),
        8 * sizeof(buffer[0]));
}

static void test_sbMemoryCopy_wordsBackwardOverlap(void** state)
{
    (void)state;
    const uint64_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer[2], 4 * sizeof(buffer[0]));
    const sb_memory_t destination_mem
        = sbMemory(&buffer, 4 * sizeof(buffer[0]));
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint64_t[]) { 3, 4, 5, 6, 5, 6, 7, 8 }),
        8 * sizeof(buffer[0]));
}

static void test_sbMemoryCopy_idempotent(void** state)
{
    (void)state;
    const uint64_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t memory = sbMemory(&buffer, 8 * sizeof(buffer[0]));
    assert_true(sbMemoryCopy(memory, memory));
    assert_memory_equal(&buffer, ((uint64_t[]) { 1, 2, 3, 4, 5, 6, 7, 8 }),
        8 * sizeof(buffer[0]));
}

static void test_sbMemoryCopy_idempotentDifferentLen(void** state)
{
    (void)state;
    const uint64_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer, 8 * sizeof(buffer[0]));
    const sb_memory_t destination_mem
        = sbMemory(&buffer, 8 * sizeof(buffer[0]));
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint64_t[]) { 1, 2, 3, 4, 5, 6, 7, 8 }),
        8 * sizeof(buffer[0]));
}

int main(void)
{
    const struct CMUnitTest tests[] = { cmocka_unit_test(
                                            test_sbMemoryValid_invalidNullPtr),
        cmocka_unit_test(test_sbMemoryValid_valid),
        cmocka_unit_test(test_sbMemoryValid_invalidZeroLen),
        cmocka_unit_test(test_sbMemory_configure),
        cmocka_unit_test(test_sbMemoryOffset_zero),
        cmocka_unit_test(test_sbMemoryOffset_outOfRange),
        cmocka_unit_test(test_sbMemoryOffset_invalid),
        cmocka_unit_test(test_sbMemoryOffset_positive),
        cmocka_unit_test(test_sbMemoryOffset_max),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalidReference),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalidReferenceAndQuery),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalidQuery),
        cmocka_unit_test(test_sbMemoryOverlapLow_simple),
        cmocka_unit_test(test_sbMemoryOverlapLow_equal),
        cmocka_unit_test(test_sbMemoryOverlapLow_equalBase),
        cmocka_unit_test(test_sbMemoryOverlapLow_equalFull),
        cmocka_unit_test(test_sbMemoryOverlapLow_superOverlap),
        cmocka_unit_test(test_sbMemoryOverlapLow_noOverlapHigh),
        cmocka_unit_test(test_sbMemoryOverlapLow_noOverlapHighTouch),
        cmocka_unit_test(test_sbMemoryOverlapLow_noOverlapLow),
        cmocka_unit_test(test_sbMemoryOverlapLow_noOverlapTouchLow),
        cmocka_unit_test(test_sbMemoryOverlapLow_overlapInside),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalidReference),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalidReferenceAndQuery),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalidQuery),
        cmocka_unit_test(test_sbMemoryOverlapHigh_simple),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equal),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equalBase),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equalEnd),
        cmocka_unit_test(test_sbMemoryOverlapHigh_superOverlap),
        cmocka_unit_test(test_sbMemoryOverlapHigh_noOverlapHigh),
        cmocka_unit_test(test_sbMemoryOverlapHigh_noOverlapHighTouch),
        cmocka_unit_test(test_sbMemoryOverlapHigh_noOverlapLow),
        cmocka_unit_test(test_sbMemoryOverlapHigh_noOverlapLowTouch),
        cmocka_unit_test(test_sbMemoryOverlapHigh_overlapInside),
        cmocka_unit_test(test_sbMemoryCopy_zeroLenOrigin),
        cmocka_unit_test(test_sbMemoryCopy_zeroLenDestination),
        cmocka_unit_test(test_sbMemoryCopy_noOverlapSingleByte),
        cmocka_unit_test(test_sbMemoryCopy_noOverlapEightByte),
        cmocka_unit_test(test_sbMemoryCopy_forwardOverlap),
        cmocka_unit_test(test_sbMemoryCopy_backwardOverlap),
        cmocka_unit_test(test_sbMemoryCopy_wordszeroLenOrigin),
        cmocka_unit_test(test_sbMemoryCopy_wordsZeroLenDestination),
        cmocka_unit_test(test_sbMemoryCopy_wordsNoOverlapSingleWord),
        cmocka_unit_test(test_sbMemoryCopy_wordsNoOverlapEightWords),
        cmocka_unit_test(test_sbMemoryCopy_wordsForwardOverlap),
        cmocka_unit_test(test_sbMemoryCopy_wordsBackwardOverlap),
        cmocka_unit_test(test_sbMemoryCopy_idempotent),
        cmocka_unit_test(test_sbMemoryCopy_idempotentDifferentLen) };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}