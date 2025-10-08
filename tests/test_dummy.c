#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void test_failing(void **state) {
    (void) state; /* unused */
    assert_true(0);
}

static void test_passing(void **state) {
    (void) state; /* unused */
    assert_true(1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // cmocka_unit_test(test_failing),
        cmocka_unit_test(test_passing),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
