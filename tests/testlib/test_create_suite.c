#include <stdlib.h>
#include <stdio.h>

#include "tests.h"
#include "asserts.h"

Test tests[] = {
    {.name="test_everything", .func=NULL},
    {.name="unit_test_1", .func=NULL},
    {.name="unit_test_2", .func=NULL},
    {.name="fail_unit_test_1", .func=NULL},
    {.name="fail_unit_test_1", .func=NULL},
    {.name="unit_fail_test_1", .func=NULL},
    {.name=NULL},
};

TestSuite const* suite;
TestOpts opts;

void after_each() {
    free_test_suite(suite);
    free_opts(&opts);
}

void test_create_suite_ok_0(void) {
    int ret = parse_test_opts(&opts, NULL, 0);
    assert_equals_int(ret, 0);

    suite = create_test_suite(&opts);
}

Test curr_tests[] = {
    {.name="test_create_suite_ok_1", .func=test_create_suite_ok_0},
};

int main(void) {
    int overall = 0;
    for (size_t i = 0; i < sizeof(curr_tests) / sizeof(Test); i++) {
        fprintf(stdout, "%s... ", curr_tests[i].name);
        fflush(stdout);
        ASSERTS_EXIT_CODE = 0;
        curr_tests[i].func();
        after_each();
        overall |= ASSERTS_EXIT_CODE;
        if (ASSERTS_EXIT_CODE == 0) {
            fprintf(stdout, "Passed!\n");
        } else break;
    }
    return overall;
}
