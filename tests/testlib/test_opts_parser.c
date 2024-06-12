#include <errno.h>

#include "tests.h"
#include "asserts.h"

Test tests[] = {
    {.name = "a", .func = NULL},
    {.name = "b", .func = NULL},
    {.name = "c", .func = NULL},
    {.name = "d", .func = NULL},
    {NULL},
};

void test_opt_parser_ok_1(void) {
    char* opts[] = {"--fail-fast", "--dry-run", "--randomize"};
    size_t opts_size = 3;

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
}

void test_opt_parser_fail_1(void) {
    char* opts[] = {"--fail-fast", "bad", "--dry-run", "--randomize"};
    size_t opts_size = 3;

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 1);
    assert_equals_int(errno, 0);
}

void test_opt_parser_fail_2(void) {
    char* opts[] = {"-v", NULL};
    size_t opts_size = 3;

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 2);
}

Test curr_tests[] = {
    {.name="test_opt_parser_ok_1", .func=test_opt_parser_ok_1},
    {.name="test_opt_parser_fail_1", .func=test_opt_parser_fail_1},
    {.name="test_opt_parser_fail_2", .func=test_opt_parser_fail_2},
};

int main(void) {
    int overall = 0;
    for (size_t i = 0; i < sizeof(curr_tests) / sizeof(Test); i++) {
        fprintf(stdout, "%s... ", curr_tests[i].name);
        ASSERTS_EXIT_CODE = 0;
        curr_tests[i].func();
        overall |= ASSERTS_EXIT_CODE;
        if (ASSERTS_EXIT_CODE == 0) {
            fprintf(stdout, "Passed!\n");
        }
    }
    return overall;
}
