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

enum OptionStrategy {
    AUTOMATIC,
    MANUAL,
} opt_strategy = AUTOMATIC;

void reset_opts() {
    opt_strategy = AUTOMATIC;
    opts = (TestOpts) {0};

    // -1 to exclude the null
    opts.included_size = (sizeof(tests) / sizeof(Test)) - 1;
    opts.included = malloc(sizeof(char*) * opts.included_size);
    assert_is_not_null(opts.included);
    assert_is_null(opts.excluded);
    assert_is_null(opts.output_file);

    for (size_t i = 0; i < opts.included_size; i++) {
        opts.included[i] = tests[i].name;
    }
}

void after_each() {
    free_test_suite(suite);

    if (opt_strategy == AUTOMATIC) {
        free_opts(&opts);
        opts.included = NULL;
        opts.included_size = 0;
    }

    if (errno) {
        fprintf(stderr, "%s\n", strerror(errno));
    }
}

// Basic test to ensure function terminates
void test_create_suite_ok_0(void) {
    reset_opts();
    suite = create_test_suite(&opts);
    assert_is_not_null(suite);
}

// Test Suite with default options
void test_create_suite_ok_1(void) {
    reset_opts();

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_is_not_null(suite->tests);
    assert_equals_int(suite->n_tests, 6);
    assert_is_not_null(suite->opts);
}

// Exclude a few tests
void test_create_suite_ok_2(void) {
    opt_strategy = MANUAL;

    char* included[] = {
        "test_everything", "fail_unit_test_1",
        "fail_unit_test_1", "unit_fail_test_1"
    };

    opts.included = included;
    opts.included_size = sizeof(included) / sizeof(char*);

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_equals_int(suite->n_tests, 4);
    assert_is_not_null(suite->tests);

    int size = (int)suite->n_tests;
    char* actual[size];

    for (int i = 0; i < size; i++) {
        actual[i] = suite->tests[i]->name;
    }

    assert_equals_str_array_unordered(actual, included, size);
}

// Include a few tests
void test_create_suite_ok_3(void) {
    opt_strategy = AUTOMATIC; // Frees resources after test

    char* options[] = {"-r", "unit_test_1", "unit_test_2"};
    size_t opts_size = sizeof(options) / sizeof(char*);

    int ret = parse_test_opts(&opts, options, opts_size);
    assert_equals_int(ret, 0);

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_equals_int(suite->n_tests, 2);
    assert_is_not_null(suite->tests);

    char** included = &options[1];

    int size = (int)suite->n_tests;
    char* actual[size];

    for (int i = 0; i < size; i++) {
        actual[i] = suite->tests[i]->name;
    }

    assert_equals_str_array_unordered(actual, included, size);
}

Test curr_tests[] = {
    {.name="test_create_suite_ok_0", .func=test_create_suite_ok_0},
    {.name="test_create_suite_ok_1", .func=test_create_suite_ok_1},
    {.name="test_create_suite_ok_2", .func=test_create_suite_ok_2},
    {.name="test_create_suite_ok_3", .func=test_create_suite_ok_3},
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
