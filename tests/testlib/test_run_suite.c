#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "tests.h"
#include "asserts.h"
#include "portability.h"

#ifdef WIN32_LEAN_AND_MEAN
#error Cannot compile this test on Windows. \
This test requires the `pipe` syscall to capture run_suite output. \
This syscall is not available on Windows, and while alternatives exist, \
they are so much more work to accomplish and I cannot be bothered. \
If you would like to help me support this on Windows, \
please submit a pull request at \
https://github.com/mkpro118/Regex-Engine/pulls
#endif

// Dummy functions to tests
int test_everything(void){return 0;}
int unit_test_1(void){return 1;}
int unit_test_2(void){return 1;}
int fail_unit_test_1(void){return 0;}
int fail_unit_test_2(void){return 1;}
int unit_fail_test_1(void){return 0;}

Test tests[] = {
    {.name="test_everything", .func=test_everything},
    {.name="unit_test_1", .func=unit_test_1},
    {.name="unit_test_2", .func=unit_test_2},
    {.name="fail_unit_test_1", .func=fail_unit_test_1},
    {.name="fail_unit_test_2", .func=fail_unit_test_2},
    {.name="unit_fail_test_1", .func=unit_fail_test_1},
    {.name=NULL},
};

// -1 to exclude the null
const size_t tests_size = sizeof(tests) / sizeof(Test) - 1;

// original stdout
int stdout_bk;
int pipefd[2];
const char* test_file_name = "testfile";

TestSuite const* suite;
TestOpts opts;

void before_each(void) {
   stdout_bk = dup(fileno(stdout));
   int x = pipe(pipefd);
   assert_equals_int(x, 0);
   dup2(pipefd[1], fileno(stdout));
}

void after_each(void) {
    free_test_suite(suite);
    free_opts(&opts);

    fflush(stdout);
    close(pipefd[1]);
    dup2(stdout_bk, fileno(stdout));
}

int test_run_suite_ok_0(void) {
    TEST_BEGIN;
    int ret = parse_test_opts(&opts, NULL, 0);
    if (ret != 0) {
        printf("%s\n", str_parse_error(ret));
        exit(1);
    }

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    int n_failures = run_test_suite(suite);
    assert_equals_int(n_failures, 3);
    TEST_END;
}

int test_run_suite_ok_1(void) {
    TEST_BEGIN;
    char* options[] = {"-x", "unit_test_2"};
    size_t options_size = sizeof(options) / sizeof(char*);

    int ret = parse_test_opts(&opts, options, options_size);

    if (ret != 0) {
        printf("%s\n", str_parse_error(ret));
        exit(1);
    }
    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    int n_failures = run_test_suite(suite);
    assert_equals_int(n_failures, 2);
    TEST_END;
}

int test_run_suite_ok_2(void) {
    TEST_BEGIN;
    char* options[] = {"-x", "unit_test_2"};
    size_t options_size = sizeof(options) / sizeof(char*);

    int ret = parse_test_opts(&opts, options, options_size);

    if (ret != 0) {
        printf("%s\n", str_parse_error(ret));
        exit(1);
    }
    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    int n_failures = run_test_suite(suite);
    assert_equals_int(n_failures, 2);
    TEST_END;
}

Test curr_tests[] = {
    {.name="test_run_suite_ok_0", .func=test_run_suite_ok_0},
    {.name="test_run_suite_ok_1", .func=test_run_suite_ok_1},
};

int main(void) {
    int overall = 0;
    for (size_t i = 0; i < sizeof(curr_tests) / sizeof(Test); i++) {
        fprintf(stdout, "%s... ", curr_tests[i].name);
        fflush(stdout);
        before_each();
        int exit_code = curr_tests[i].func();
        after_each();
        overall |= exit_code;
        if (exit_code == 0) {
            fprintf(stdout, "Passed!\n");
        } else break;
    }
    return overall;
}
