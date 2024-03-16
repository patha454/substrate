

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include "buffer.h"

static void test_sbBufferValid_invalid_null(void** state)
{
    (void) state;
    assert_false(sbBufferValid(nullptr));
}

static void test_sbBufferValid_invalid_null_base(void** state)
{
    (void) state;
    struct SbBuffer buffer = {
            .base = (uintptr_t) (void*) nullptr,
            .stride = 1,
            .length = 1
    };
    assert_false(sbBufferValid(&buffer));
}

static void test_sbBufferValid_invalid_zero_length(void** state)
{
    (void) state;
    const size_t length = 0;
    uint8_t memory[length];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = 1,
            .length = length
    };
    assert_false(sbBufferValid(&buffer));
}

static void test_sbBufferValid_invalid_zero_stride(void** state)
{
    (void) state;
    const size_t length = 8;
    uint8_t memory[length];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = 0,
            .length = length
    };
    assert_false(sbBufferValid(&buffer));
}

static void test_sbBufferValid_invalid_stride_gt_length(void** state)
{
    (void) state;
    const size_t length = 8;
    uint8_t memory[length];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = length + 1,
            .length = length
    };
    assert_false(sbBufferValid(&buffer));
}

static void test_sbBufferValid_valid(void** state)
{
    (void) state;
    const int size = 16;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = 1,
            .length = size,
    };
    assert_true(sbBufferValid(&buffer));
}

static void test_sbBufferValid_valid_stride_eq_length(void** state)
{
    (void) state;
    const int size = 16;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = size,
            .length = size,
    };
    assert_true(sbBufferValid(&buffer));
}

static void test_sbBufferCapacity_stride_eq_size(void** state)
{
    (void) state;
    const int size = 16;
    const int stride = size;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size
    };
    assert_int_equal(sbBufferCapacity(&buffer), size / stride);
}

static void test_sbBufferCapacity_size_double_stride(void** state)
{
    (void) state;
    const int size = 16;
    const int stride = size / 2;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_int_equal(sbBufferCapacity(&buffer), size / stride);
}

static void test_sbBufferCapacity_size_gt_stride(void** state)
{
    (void) state;
    const int size = 16;
    const int stride = size - 1;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_int_equal(sbBufferCapacity(&buffer), size / stride);
}

static void test_sbBufferCapacity_size_lt_stride(void** state)
{
    (void) state;
    const int size = 16;
    const int stride = size + 1;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_int_equal(sbBufferCapacity(&buffer), 0);
}

static void test_sbBufferCapacity_null_buffer(void** state)
{
    (void) state;
    assert_int_equal(sbBufferCapacity(nullptr), 0);
}

static void test_sbBufferIndex_null_buffer(void** state)
{
    assert_null(sbBufferIndex(nullptr, 0));
    assert_null(sbBufferIndex(nullptr, 1));
    assert_null(sbBufferIndex(nullptr, 2048));
}

static void test_sbBufferIndex_end_of_range(void** state)
{
    const int size = 1;
    const int stride = size + 1;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_null(sbBufferIndex(&buffer, 1));
}

static void test_sbBufferIndex_out_of_range(void** state)
{
    const int size = 1;
    const int stride = size + 1;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_null(sbBufferIndex(&buffer, 2));
}

static void test_sbBufferIndex_zero_index(void** state)
{
    const int size = 8;
    const int stride = 1;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_ptr_equal(sbBufferIndex(&buffer, 0), &memory);
}

static void test_sbBufferIndex_zero_index_large_stride(void** state)
{
    const int size = 8;
    const int stride = 4;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_ptr_equal(sbBufferIndex(&buffer, 0), &memory);
}

static void test_sbBufferIndex_mid_index(void** state)
{
    const int size = 8;
    const int stride = 1;
    const int index = 4;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_ptr_equal(sbBufferIndex(&buffer, index), memory + (index * stride));
}

static void test_sbBufferIndex_mid_index_large_stride(void** state)
{
    const int size = 8;
    const int stride = 2;
    const int index = 2;
    uint8_t memory[size];
    struct SbBuffer buffer = {
            .base = (uintptr_t) &memory,
            .stride = stride,
            .length = size,
    };
    assert_ptr_equal(sbBufferIndex(&buffer, index), memory + (stride * index));
}


int main(void ) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_sbBufferValid_invalid_null),
            cmocka_unit_test(test_sbBufferValid_invalid_null_base),
            cmocka_unit_test(test_sbBufferValid_invalid_zero_length),
            cmocka_unit_test(test_sbBufferValid_invalid_zero_stride),
            cmocka_unit_test(test_sbBufferValid_invalid_stride_gt_length),
            cmocka_unit_test(test_sbBufferValid_valid),
            cmocka_unit_test(test_sbBufferValid_valid_stride_eq_length),
            cmocka_unit_test(test_sbBufferCapacity_stride_eq_size),
            cmocka_unit_test(test_sbBufferCapacity_size_double_stride),
            cmocka_unit_test(test_sbBufferCapacity_size_gt_stride),
            cmocka_unit_test(test_sbBufferCapacity_size_lt_stride),
            cmocka_unit_test(test_sbBufferCapacity_null_buffer),
            cmocka_unit_test(test_sbBufferIndex_null_buffer),
            cmocka_unit_test(test_sbBufferIndex_end_of_range),
            cmocka_unit_test(test_sbBufferIndex_out_of_range),
            cmocka_unit_test(test_sbBufferIndex_zero_index),
            cmocka_unit_test(test_sbBufferIndex_zero_index_large_stride),
            cmocka_unit_test(test_sbBufferIndex_mid_index),
            cmocka_unit_test(test_sbBufferIndex_mid_index_large_stride)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
