#include "memory.h"

// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

static void test_sbMemoryValid_invalid_nullptr(void** state)
{
    (void)state;
    const struct _SbMemory memory = { .base = (intptr_t)(void*)nullptr, 1 };
    assert_false(sbMemoryValid(memory));
}

static void test_sbMemoryValid_invalid_zero_len(void** state)
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

static void test_sbMemoryOffset_out_of_range(void** state)
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

static void test_sbMemoryOverlapLow_invalid_reference(void** state)
{
    (void)state;
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t query_bytes = { 0 };
    const struct _SbMemory query = sbMemory(&query_bytes, sizeof(query_bytes));
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_invalid_query(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t reference_bytes = { 0 };
    const struct _SbMemory reference
        = sbMemory(&reference_bytes, sizeof(reference_bytes));
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_invalid_reference_and_query(void** state)
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

static void test_sbMemoryOverlapLow_equal_base(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_equal_full(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_super_overlap(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_no_overlap_low(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[4], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_no_overlap_high(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[4], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_no_overlap_low_touch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_no_overlap_high_touch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapLow_overlap_inside(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 8);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapLow(reference, query));
}

static void test_sbMemoryOverlapHigh_invalid_reference(void** state)
{
    (void)state;
    const struct _SbMemory reference
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t query_bytes = { 0 };
    const struct _SbMemory query = sbMemory(&query_bytes, sizeof(query_bytes));
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_invalid_query(void** state)
{
    (void)state;
    const struct _SbMemory query
        = { .base = (intptr_t)(void*)nullptr, .length = 0 };
    const uint8_t reference_bytes = { 0 };
    const struct _SbMemory reference
        = sbMemory(&reference_bytes, sizeof(reference_bytes));
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_invalid_reference_and_query(void** state)
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

static void test_sbMemoryOverlapHigh_equal_base(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 4);
    const sb_memory_t query = sbMemory(&bytes, 6);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_equal_end(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 4);
    const sb_memory_t query = sbMemory(&bytes, 6);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_super_overlap(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 8);
    assert_true(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_no_overlap_low(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[4], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_no_overlap_high(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[4], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_no_overlap_low_touch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes[2], 2);
    const sb_memory_t query = sbMemory(&bytes, 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_no_overlap_high_touch(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 2);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryOverlapHigh_overlap_inside(void** state)
{
    (void)state;
    const uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t reference = sbMemory(&bytes, 8);
    const sb_memory_t query = sbMemory(&bytes[2], 2);
    assert_false(sbMemoryOverlapHigh(reference, query));
}

static void test_sbMemoryCopy_zero_len_origin(void** state)
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

static void test_sbMemoryCopy_zero_len_destination(void** state)
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

static void test_sbMemoryCopy_no_overlap_single_byte(void** state)
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

static void test_sbMemoryCopy_no_overlap_eight_byte(void** state)
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

static void test_sbMemoryCopy_forward_overlap(void** state)
{
    (void)state;
    const uint8_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer, 4);
    const sb_memory_t destination_mem = sbMemory(&buffer[2], 4);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint8_t[]) { 1, 2, 1, 2, 3, 4, 7, 8 }), 8);
}

static void test_sbMemoryCopy_backward_overlap(void** state)
{
    (void)state;
    const uint8_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const sb_memory_t origin_mem = sbMemory(&buffer[2], 4);
    const sb_memory_t destination_mem = sbMemory(&buffer, 4);
    assert_true(sbMemoryCopy(origin_mem, destination_mem));
    assert_memory_equal(&buffer, ((uint8_t[]) { 3, 4, 5, 6, 5, 6, 7, 8 }), 8);
}

static void test_sbMemoryCopy_words_zero_len_origin(void** state)
{
    (void)state;
    const uint64_t origin[] = { 1 };
    const uint64_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, 0);
    const sb_memory_t destination_mem = sbMemory(&destination, 1);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_words_zero_len_destination(void** state)
{
    (void)state;
    const uint64_t origin[] = { 1 };
    const uint64_t destination[] = { 5 };
    const sb_memory_t origin_mem = sbMemory(&origin, 1);
    const sb_memory_t destination_mem = sbMemory(&destination, 0);
    assert_false(sbMemoryCopy(origin_mem, destination_mem));
    assert_int_equal(destination[0], 5);
}

static void test_sbMemoryCopy_words_no_overlap_single_word(void** state)
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

static void test_sbMemoryCopy_words_no_overlap_eight_words(void** state)
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

static void test_sbMemoryCopy_words_forward_overlap(void** state)
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

static void test_sbMemoryCopy_words_backward_overlap(void** state)
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

static void test_sbMemoryCopy_idempotent_different_len(void** state)
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
                                            test_sbMemoryValid_invalid_nullptr),
        cmocka_unit_test(test_sbMemoryValid_valid),
        cmocka_unit_test(test_sbMemoryValid_invalid_zero_len),
        cmocka_unit_test(test_sbMemory_configure),
        cmocka_unit_test(test_sbMemoryOffset_zero),
        cmocka_unit_test(test_sbMemoryOffset_out_of_range),
        cmocka_unit_test(test_sbMemoryOffset_invalid),
        cmocka_unit_test(test_sbMemoryOffset_positive),
        cmocka_unit_test(test_sbMemoryOffset_max),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalid_reference),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalid_reference_and_query),
        cmocka_unit_test(test_sbMemoryOverlapLow_invalid_query),
        cmocka_unit_test(test_sbMemoryOverlapLow_simple),
        cmocka_unit_test(test_sbMemoryOverlapLow_equal),
        cmocka_unit_test(test_sbMemoryOverlapLow_equal_base),
        cmocka_unit_test(test_sbMemoryOverlapLow_equal_full),
        cmocka_unit_test(test_sbMemoryOverlapLow_super_overlap),
        cmocka_unit_test(test_sbMemoryOverlapLow_no_overlap_high),
        cmocka_unit_test(test_sbMemoryOverlapLow_no_overlap_high_touch),
        cmocka_unit_test(test_sbMemoryOverlapLow_no_overlap_low),
        cmocka_unit_test(test_sbMemoryOverlapLow_no_overlap_low_touch),
        cmocka_unit_test(test_sbMemoryOverlapLow_overlap_inside),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalid_reference),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalid_reference_and_query),
        cmocka_unit_test(test_sbMemoryOverlapHigh_invalid_query),
        cmocka_unit_test(test_sbMemoryOverlapHigh_simple),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equal),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equal_base),
        cmocka_unit_test(test_sbMemoryOverlapHigh_equal_end),
        cmocka_unit_test(test_sbMemoryOverlapHigh_super_overlap),
        cmocka_unit_test(test_sbMemoryOverlapHigh_no_overlap_high),
        cmocka_unit_test(test_sbMemoryOverlapHigh_no_overlap_high_touch),
        cmocka_unit_test(test_sbMemoryOverlapHigh_no_overlap_low),
        cmocka_unit_test(test_sbMemoryOverlapHigh_no_overlap_low_touch),
        cmocka_unit_test(test_sbMemoryOverlapHigh_overlap_inside),
        cmocka_unit_test(test_sbMemoryCopy_zero_len_origin),
        cmocka_unit_test(test_sbMemoryCopy_zero_len_destination),
        cmocka_unit_test(test_sbMemoryCopy_no_overlap_single_byte),
        cmocka_unit_test(test_sbMemoryCopy_no_overlap_eight_byte),
        cmocka_unit_test(test_sbMemoryCopy_forward_overlap),
        cmocka_unit_test(test_sbMemoryCopy_backward_overlap),
        cmocka_unit_test(test_sbMemoryCopy_words_zero_len_origin),
        cmocka_unit_test(test_sbMemoryCopy_words_zero_len_destination),
        cmocka_unit_test(test_sbMemoryCopy_words_no_overlap_single_word),
        cmocka_unit_test(test_sbMemoryCopy_words_no_overlap_eight_words),
        cmocka_unit_test(test_sbMemoryCopy_words_forward_overlap),
        cmocka_unit_test(test_sbMemoryCopy_words_backward_overlap),
        cmocka_unit_test(test_sbMemoryCopy_idempotent),
        cmocka_unit_test(test_sbMemoryCopy_idempotent_different_len) };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}