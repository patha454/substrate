#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "substrate/memory/span.h"

static void test_sbCreateSpan_null_zero(void **state) {
    (void) state;
    const size_t len = 0;
    SbSpan span = sbCreateSpan(nullptr, len);
    assert_ptr_equal((void *) nullptr, span.ptr);
    assert_int_equal(span.len, len);
}

static void test_sbCreateSpan_null_non_zero(void **state) {
    (void) state;
    const size_t len = 1;
    SbSpan span = sbCreateSpan(nullptr, len);
    assert_ptr_equal((void *) nullptr, span.ptr);
    assert_int_equal(span.len, 0);
}

static void test_sbCreateSpan_non_null_zero(void **state) {
    (void) state;
    void * ptr = (void *) 0x12345678;
    const size_t len = 0;
    SbSpan span = sbCreateSpan(ptr, len);
    assert_ptr_equal(ptr, span.ptr);
    assert_int_equal(span.len, len);
}

static void test_sbCreateSpan_non_null_non_zero(void **state) {
    (void) state;
    void * ptr = (void *) 0x12345678;
    const size_t len = 1;
    SbSpan span = sbCreateSpan(ptr, len);
    assert_ptr_equal(ptr, span.ptr);
    assert_int_equal(span.len, len);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sbCreateSpan_null_zero),
        cmocka_unit_test(test_sbCreateSpan_null_non_zero),
        cmocka_unit_test(test_sbCreateSpan_non_null_zero),
        cmocka_unit_test(test_sbCreateSpan_non_null_non_zero)
    };
    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
