#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "substrate/memory/span.h"

/* A span to a null pointer must be legal. */
static void test_sbCreateSpan_null_zero(void ** state) {
    (void) state;
    const size_t len = 0;
    const SbSpan span = sbCreateSpan(nullptr, len);
    assert_ptr_equal((void *) nullptr, span.ptr);
    assert_int_equal(span.len, len);
}

/* A span to a null pointer must have its length set to zero. */
static void test_sbCreateSpan_null_non_zero(void ** state) {
    (void) state;
    const size_t len = 1;
    SbSpan span = sbCreateSpan(nullptr, len);
    assert_ptr_equal((void *) nullptr, span.ptr);
    assert_int_equal(span.len, 0);
}

/* A span to zero length memory must be legal. */
static void test_sbCreateSpan_non_null_zero(void ** state) {
    (void) state;
    void * ptr = (void *) 0x12345678;
    const size_t len = 0;
    SbSpan span = sbCreateSpan(ptr, len);
    assert_ptr_equal(ptr, span.ptr);
    assert_int_equal(span.len, len);
}

/* A span to a region of memory but be created accurately. */
static void test_sbCreateSpan_non_null_non_zero(void ** state) {
    (void) state;
    void * ptr = (void *) 0x12345678;
    const size_t len = 1;
    const SbSpan span = sbCreateSpan(ptr, len);
    assert_ptr_equal(ptr, span.ptr);
    assert_int_equal(span.len, len);
}

/* Copying a zero-length span is legal and must copy no data. */
static void test_sbCopySpan_zero_length_origin(void ** state) {
    (void) state;
    uint8_t origin[] = {};
    uint8_t destination[] = {2, 2, 2, 2, 2};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(destination));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(destination); i++) {
        assert_int_equal(destination[i], 2);
    }
}

/* Copying to a zero-length span is legal and must copy no data. */
static void test_sbCopySpan_zero_length_destination(void ** state) {
    (void) state;
    uint8_t origin[] = {2, 2, 2, 2, 2, 2};
    uint8_t destination[] = {4, 4, 4, 4, 4, 4};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(destination, 0);
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(destination); i++) {
        assert_int_equal(destination[i], 4);
    }
}

/* Copying from a null span is legal, but must copy no data. */
static void test_sbCopySpan_null_origin(void ** state) {
    (void) state;
    uint8_t destination[] = {4, 4, 4, 4, 4, 4};
    const SbSpan originSpan = sbCreateSpan(nullptr, 0);
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(destination));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(destination); i++) {
        assert_int_equal(destination[i], 4);
    }
}

/* Copying to a null span is legal, but must copy no data. */
static void test_sbCopySpan_null_destination(void ** state) {
    (void) state;
    uint8_t origin[] = {4, 4, 4, 4, 4, 4};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(nullptr, 0);
    assert_true(sbCopySpan(originSpan, destinationSpan));
}

/* A trivial copy. */
static void test_sbCopySpan_small_no_overlap_equal_length(void ** state) {
    (void) state;
    uint8_t origin[] = {1, 2, 3, 4};
    uint8_t destination[] = {5, 6, 7, 8};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(destination));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(origin); i++) {
        assert_int_equal(origin[i], destination[i]);
    }
}

/* Copying to a larger destination should leave the end of the destination untouched. */
static void test_sbCopySpan_small_no_overlap_longer_destination(void ** state) {
    (void) state;
    uint8_t origin[] = {1, 2, 3, 4};
    uint8_t destination[] = {5, 6, 7, 8, 9, 10};
    const uint8_t expected[] = {1, 2, 3, 4, 9, 10};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(destination));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(destination[i], expected[i]);
    }
}

/* Copying to a shorter destination should truncate the copied data. */
static void test_sbCopySpan_small_no_overlap_longer_origin(void ** state) {
    (void) state;
    uint8_t origin[] = {1, 2, 3, 4};
    uint8_t destination[] = {5, 6, 7, 8, 9};
    const uint8_t expected[] = {1, 2, 7, 8, 9};
    const SbSpan originSpan = sbCreateSpan(origin, 2);
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(expected));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(destination[i], expected[i]);
    }
}

/* An idempotent copy is legal and must not corrupt memory. */
static void test_sbCopySpan_small_idempotent(void ** state) {
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7};
    const uint8_t expected[] = {1, 2, 3, 4, 5, 6, 7};
    const SbSpan origin = sbCreateSpan(buffer, sizeof(origin));
    assert_true(sbCopySpan(origin, origin));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(buffer[i], expected[i]);
    }
}

/*
 * A copy where the end of the destination overlaps the start of the origin
 * must place the complete, original data in the new location.
 */
static void test_sbCopySpan_small_lower_overlap(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7};
    const uint8_t expected[] = {3, 4, 5, 6, 5, 6, 7 };
    const SbSpan originSpan = sbCreateSpan(&buffer[2], 4);
    const SbSpan destinationSpan = sbCreateSpan(buffer, 4);
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(((uint8_t *) destinationSpan.ptr)[i], expected[i]);
    }
}

/*
 * A copy where the end of the origin overlaps the start of the destination
 * must place the complete, original data into the new location.
 */
static void test_sbCopySpan_small_upper_overlap(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7};
    const uint8_t expected[] = {1, 2, 1, 2, 3, 4, 7};
    const SbSpan originSpan = sbCreateSpan(buffer, 4);
    const SbSpan destinationSpan = sbCreateSpan(&buffer[2], 4);
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(buffer[i], expected[i]);
    }
}

/*
 * A copy where the destination spans a superset of the origin must place the
 * original, complete data in the new location.
 */
static void test_sbCopySpan_small_destination_superset_middle(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const uint8_t expected[] = {3, 4, 5, 6, 5, 6, 7, 8};
    const SbSpan originSpan = sbCreateSpan(&buffer[2], 4);
    const SbSpan destinationSpan = sbCreateSpan(buffer, sizeof(buffer));
    assert_true(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(buffer[i], expected[i]);
    }
}

/*
 * A copy where the destination spans a subset of the origin must be rejected
 * without modifying the data - the destination is smaller than the origin.
 */
static void test_sbCopySpan_small_destination_subset_middle(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const uint8_t expected[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const SbSpan originSpan = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan destinationSpan = sbCreateSpan(&buffer[2], 4);
    assert_false(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(expected); i++) {
        assert_int_equal(buffer[i], expected[i]);
    }
}


/*
 * A copy to a smaller destination must be rejected without modifying the data.
 *
 */
static void test_sbCopySpan_small_no_overlap_smaller_destination(
    void ** state
) {
    (void) state;
    uint8_t origin[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t destination[] = {9, 10, 11, 12};
    uint8_t expected[] = {9, 10, 11, 12};
    const SbSpan originSpan = sbCreateSpan(origin, sizeof(origin));
    const SbSpan destinationSpan = sbCreateSpan(destination, sizeof(destination));
    assert_false(sbCopySpan(originSpan, destinationSpan));
    for (size_t i = 0; i < sizeof(destination); i++) {
        assert_int_equal(destination[i], expected[i]);
    }
}

/* A total slice of the original array must be a full view of the original. */
static void test_sbSliceSpan_idempotent(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, 0, sizeof(buffer));
    assert_int_equal(span.len, slice.len);
    assert_ptr_equal(span.ptr, slice.ptr);
    for (size_t i = 0; i < sizeof(buffer); i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i], ((uint8_t *) slice.ptr)[i]);
    }
}

/* A slice from the start of a span must be legal. */
static void test_sbSliceSpan_start_partial(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceLength = sizeof(buffer) - 2;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, 0, sliceLength);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr, slice.ptr);
    for (size_t i = 0; i < sliceLength; i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i], ((uint8_t *) slice.ptr)[i]);
    }
}

/*
 * A slice from the start of a span which exceeds the original length must be
 * legal but silently truncated to the original span length.
 */
static void test_sbSliceSpan_start_overflow(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t spanLength = sizeof(buffer) + 2;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, 0, spanLength);
    assert_int_equal(slice.len, span.len);
    assert_ptr_equal(span.ptr, slice.ptr);
    for (size_t i = 0; i < slice.len; i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i], ((uint8_t *) slice.ptr)[i]);
    }
}

/* A slice in the middle of the span must be legal. */
static void test_sbSliceSpan_middle(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceLength = 2;
    const size_t sliceOffset = 3;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, sliceOffset, sliceLength);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr + sliceOffset, slice.ptr);
    for (size_t i = 0; i < sliceLength; i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i + sliceOffset], ((uint8_t *) slice.ptr)[i]);
    }
}

/* A slice in the middle of the span to the end must be legal. */
static void test_sbSliceSpan_middle_end(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceOffset = 2;
    const size_t sliceLength = sizeof(buffer) - sliceOffset;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, sliceOffset, sliceLength);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr + sliceOffset, slice.ptr);
    for (size_t i = 0; i < sliceLength; i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i + sliceOffset], ((uint8_t *) slice.ptr)[i]);
    }
}

/*
 * A slice in the middle which overflows the original must be legal but
 * silently truncated.
 */
static void test_sbSliceSpan_middle_overflow(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceOffset = 2;
    const size_t sliceLength = sizeof(buffer) - sliceOffset;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, sliceOffset, sliceLength + 3);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr + sliceOffset, slice.ptr);
    for (size_t i = 0; i < sliceLength; i++) {
        assert_int_equal(((uint8_t *) span.ptr)[i + sliceOffset], ((uint8_t *) slice.ptr)[i]);
    }
}

/* A zero-length slice from the end of a span must be legal. */
static void test_sbSliceSpan_end_zero(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceOffset = sizeof(buffer);
    const size_t sliceLength = 0;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, sliceOffset, sliceLength);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr + sliceOffset, slice.ptr);
}

/* A non-zero-length slice from the end of a span must be legal. */
static void test_sbSliceSpan_end_overflow(void ** state) {
    (void) state;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const size_t sliceOffset = sizeof(buffer);
    const size_t sliceLength = 0;
    const SbSpan span = sbCreateSpan(buffer, sizeof(buffer));
    const SbSpan slice = sbSliceSpan(span, sliceOffset, sliceLength + 1);
    assert_int_equal(slice.len, sliceLength);
    assert_ptr_equal(span.ptr + sliceOffset, slice.ptr);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sbCreateSpan_null_zero),
        cmocka_unit_test(test_sbCreateSpan_null_non_zero),
        cmocka_unit_test(test_sbCreateSpan_non_null_zero),
        cmocka_unit_test(test_sbCreateSpan_non_null_non_zero),
        cmocka_unit_test(test_sbCopySpan_zero_length_origin),
        cmocka_unit_test(test_sbCopySpan_zero_length_destination),
        cmocka_unit_test(test_sbCopySpan_null_origin),
        cmocka_unit_test(test_sbCopySpan_null_destination),
        cmocka_unit_test(test_sbCopySpan_small_no_overlap_equal_length),
        cmocka_unit_test(test_sbCopySpan_small_no_overlap_longer_destination),
        cmocka_unit_test(test_sbCopySpan_small_no_overlap_longer_origin),
        cmocka_unit_test(test_sbCopySpan_small_lower_overlap),
        cmocka_unit_test(test_sbCopySpan_small_upper_overlap),
        cmocka_unit_test(test_sbCopySpan_small_idempotent),
        cmocka_unit_test(test_sbCopySpan_small_destination_superset_middle),
        cmocka_unit_test(test_sbCopySpan_small_destination_subset_middle),
        cmocka_unit_test(test_sbCopySpan_small_no_overlap_smaller_destination),
        cmocka_unit_test(test_sbSliceSpan_idempotent),
        cmocka_unit_test(test_sbSliceSpan_start_partial),
        cmocka_unit_test(test_sbSliceSpan_start_overflow),
        cmocka_unit_test(test_sbSliceSpan_middle),
        cmocka_unit_test(test_sbSliceSpan_middle_end),
        cmocka_unit_test(test_sbSliceSpan_middle_overflow),
        cmocka_unit_test(test_sbSliceSpan_end_zero),
        cmocka_unit_test(test_sbSliceSpan_end_overflow)
    };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
