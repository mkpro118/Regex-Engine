#include <errno.h>

#include "tests.h"
#include "asserts.h"

Test tests[] = {
    {.name = "a", .func = NULL},
    {.name = "b", .func = NULL},
    {.name = "c", .func = NULL},
    {.name = "d", .func = NULL},
    {.name=NULL},
};

void test_opt_parser_ok_1(void) {
    char* opts[] = {"--fail-fast", "--dry-run", "--randomize"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
}

void test_opt_parser_fail_1(void) {
    char* opts[] = {"--fail-fast", "bad", "--dry-run", "--randomize"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 1);
    assert_equals_int(errno, 0);
}

void test_opt_parser_fail_2(void) {
    char* opts[] = {"-v", NULL};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 2);
}

void test_opt_parser_fail_3(void) {
    char* opts[] = {"-v"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 3);
}

void test_opt_parser_fail_4(void) {
    char* opts[] = {"--verbose", "3"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 4);
}

void test_opt_parser_fail_5(void) {
    char* opts[] = {"--run"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 5);
}

void test_opt_parser_fail_6(void) {
    char* opts[] = {"-r", "test_func"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 6);
}

void test_opt_parser_fail_7(void) {
    char* opts[] = {"-x"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 7);
}

void test_opt_parser_fail_8(void) {
    char* opts[] = {"--exclude", "test_func"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 8);
}

void test_opt_parser_fail_9(void) {
    char* opts[] = {"-o"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 9);
}

void test_opt_parser_fail_10(void) {
    char* opts[] = {"-t"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 10);
}

void test_opt_parser_fail_11(void) {
    char* opts[] = {"--timeout", "bad"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 11);
}

void test_opt_parser_fail_12(void) {
    char* opts[] = {"--summary", "-v", "1"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    TestOpts opts_buf;

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 12);
}

Test curr_tests[] = {
    {.name="test_opt_parser_ok_1", .func=test_opt_parser_ok_1},
    {.name="test_opt_parser_fail_1", .func=test_opt_parser_fail_1},
    {.name="test_opt_parser_fail_2", .func=test_opt_parser_fail_2},
    {.name="test_opt_parser_fail_3", .func=test_opt_parser_fail_3},
    {.name="test_opt_parser_fail_4", .func=test_opt_parser_fail_4},
    {.name="test_opt_parser_fail_5", .func=test_opt_parser_fail_5},
    {.name="test_opt_parser_fail_6", .func=test_opt_parser_fail_6},
    {.name="test_opt_parser_fail_7", .func=test_opt_parser_fail_7},
    {.name="test_opt_parser_fail_8", .func=test_opt_parser_fail_8},
    {.name="test_opt_parser_fail_9", .func=test_opt_parser_fail_9},
    {.name="test_opt_parser_fail_10", .func=test_opt_parser_fail_10},
    {.name="test_opt_parser_fail_11", .func=test_opt_parser_fail_11},
    {.name="test_opt_parser_fail_12", .func=test_opt_parser_fail_12},
};

int main(void) {
    int overall = 0;
    for (size_t i = 0; i < sizeof(curr_tests) / sizeof(Test); i++) {
        fprintf(stdout, "%s... ", curr_tests[i].name);
        fflush(stdout);
        ASSERTS_EXIT_CODE = 0;
        curr_tests[i].func();
        overall |= ASSERTS_EXIT_CODE;
        if (ASSERTS_EXIT_CODE == 0) {
            fprintf(stdout, "Passed!\n");
        } else break;
    }
    return overall;
}
const char* __asan_default_options() { return "detect_leaks=0"; }
