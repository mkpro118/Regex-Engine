#include <errno.h>

#include "tests.h"
#include "asserts.h"

// Dummy set of test functions
Test tests[] = {
    {.name = "a", .func = NULL},
    {.name = "b", .func = NULL},
    {.name = "c", .func = NULL},
    {.name = "d", .func = NULL},
    {.name=NULL},
};

// Options are parsed into this buffer
TestOpts opts_buf;

// Releases resources after every test
static inline void after_each(void) {
    free_opts(&opts_buf);
}

// Tests that parser works successfully on good input
// Options tested: --fail-fast/-ff, --dry-run/-d, --randomize/-z
void test_opt_parser_ok_1(void) {
    char* opts[] = {"--fail-fast", "--dry-run", "-z"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
}

// Tests that parser sets boolean values correctly on good input
// Options tested: --fail-fast/-ff, --dry-run/-d, --randomize/-z
void test_opt_parser_ok_2(void) {
    char* opts[] = {"-ff", "-d", "--randomize"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.fail_fast, 1);
    assert_equals_int(opts_buf.dry_run, 1);
    assert_equals_int(opts_buf.verbose, 1);
    assert_equals_int(opts_buf.randomize, 1);
}

// Another test that verifies boolean values are correctly set on good input
// Options tested: --fail-fast/-ff, --summary/-s
void test_opt_parser_ok_3(void) {
    char* opts[] = {"--fail-fast", "--summary"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.fail_fast, 1);
    assert_equals_int(opts_buf.dry_run, 0);
    assert_equals_int(opts_buf.randomize, 0);
    assert_equals_int(opts_buf.summary, 1);
    assert_equals_int(opts_buf.verbose, 0);
    assert_equals_int(opts_buf.included_size, 4);
    assert_equals_int(opts_buf.excluded_size, 0);
}

// Test verbosity is parsed correctly
// Options tested: --verbose/-v
void test_opt_parser_ok_4(void) {
    char* opts[] = {"-v", "2"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.verbose, 2);
}

// Test timeout is parsed correctly
// Options tested: --timeout/-t
void test_opt_parser_ok_5(void) {
    char* opts[] = {"--timeout", "2000"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.timeout, 2000);
}

// Tests that the parser correctly includes and excludes tests
// Options tested: --exclude/-x
// Implicitly tests inclusion too
void test_opt_parser_ok_6(void) {
    char* opts[] = {"-x", "c", "b"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);
    char* expected_included[] = {"a", "d"};
    char* expected_excluded[] = {"b", "c"};

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.included_size, 2);
    assert_equals_int(opts_buf.excluded_size, 2);
    assert_equals_str_array_unordered(opts_buf.included, expected_included, 2);
    assert_equals_str_array_unordered(opts_buf.excluded, expected_excluded, 2);
}

// Tests that the parser correctly parses the output file
// Options tested: --output/-o
void test_opt_parser_ok_7(void) {
    char* opts[] = {"-o", "test_file"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 0);
    assert_equals_str(opts_buf.output_file, "test_file");
}

// Tests that the parser correctly lists the included tests
// when specified multiple times
// Options tested: --run/-r
void test_opt_parser_ok_8(void) {
    char* opts[] = {"-r", "a", "-r", "c", "-r", "d"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);
    char* expected[] = {"a", "c", "d"};

    assert_equals_int(ret, 0);
    assert_equals_int(opts_buf.included_size, 3);
    assert_equals_str_array_unordered(expected, opts_buf.included, 3);
}

// Test parse error: Unsupported option
void test_opt_parser_fail_1(void) {
    char* opts[] = {"--fail-fast", "bad", "--dry-run", "--randomize"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 1);
    assert_equals_int(errno, 0);
}

// Test parse error: no value to --verbose
void test_opt_parser_fail_3(void) {
    char* opts[] = {"-v"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 3);
}

// Test parse error: bad value to --verbose
void test_opt_parser_fail_4(void) {
    char* opts[] = {"--verbose", "3"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 4);
}

// Test parse error: no value to --run
void test_opt_parser_fail_5(void) {
    char* opts[] = {"--run"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 5);
}

// Test parse error: bad value to --run
void test_opt_parser_fail_6(void) {
    char* opts[] = {"-r", "test_func"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 6);
}

// Test parse error: no value to --exclude
void test_opt_parser_fail_7(void) {
    char* opts[] = {"-x"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 7);
}

// Test parse error: bad value to --exclude
void test_opt_parser_fail_8(void) {
    char* opts[] = {"--exclude", "test_func"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 8);
}

// Test parse error: no value to --output
void test_opt_parser_fail_9(void) {
    char* opts[] = {"-o"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 9);
}

// Test parse error: no value to --timeout
void test_opt_parser_fail_10(void) {
    char* opts[] = {"-t"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 10);
}

// Test parse error: bad value to --timeout
void test_opt_parser_fail_11(void) {
    char* opts[] = {"--timeout", "bad"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 11);
}

// Test parse error: using incompatible options verbose and summary
void test_opt_parser_fail_12(void) {
    char* opts[] = {"--summary", "-v", "1"};
    size_t opts_size = sizeof(opts) / sizeof(char*);

    int ret = parse_test_opts(&opts_buf, opts, opts_size);

    assert_equals_int(ret, 12);
}

Test curr_tests[] = {
    {.name="test_opt_parser_ok_1", .func=test_opt_parser_ok_1},
    {.name="test_opt_parser_ok_2", .func=test_opt_parser_ok_2},
    {.name="test_opt_parser_ok_3", .func=test_opt_parser_ok_3},
    {.name="test_opt_parser_ok_4", .func=test_opt_parser_ok_4},
    {.name="test_opt_parser_ok_5", .func=test_opt_parser_ok_5},
    {.name="test_opt_parser_ok_6", .func=test_opt_parser_ok_6},
    {.name="test_opt_parser_ok_7", .func=test_opt_parser_ok_7},
    {.name="test_opt_parser_ok_8", .func=test_opt_parser_ok_8},
    {.name="test_opt_parser_fail_1", .func=test_opt_parser_fail_1},
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
        after_each();
        overall |= ASSERTS_EXIT_CODE;
        if (ASSERTS_EXIT_CODE == 0) {
            fprintf(stdout, "Passed!\n");
        } else break;
    }
    return overall;
}
