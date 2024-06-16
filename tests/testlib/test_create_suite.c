#include <stdlib.h>
#include <stdio.h>

#include "tests.h"
#include "asserts.h"

// Dummy functions to tests
int test_everything(void) { return 0; }
int unit_test_1(void) { return 0; }
int unit_test_2(void) { return 0; }
int fail_unit_test_1(void) { return 0; }
int fail_unit_test_2(void) { return 0; }
int unit_fail_test_1(void) { return 0; }

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

// Sort suite tests relatively to global variable tests
int test_sort_compar(const void* a, const void* b) {
    const Test* ta = *((const Test**) a);
    const Test* tb = *((const Test**) b);

    int idxa = -1;
    int idxb = -1;
    for (size_t i = 0; i < tests_size; i++) {
        if (strcmp(tests[i].name, ta->name) == 0) {idxa=i;}
        if (strcmp(tests[i].name, tb->name) == 0) {idxb=i;}
    }

    if (idxa < 0) {
        fprintf(stderr, "TEST ERROR: (A) Didn't find %s %p\n", ta->name, (void*)ta);
        exit(1);
    }
    if (idxb < 0) {
        fprintf(stderr, "TEST ERROR: (B) Didn't find %s\n", tb->name);
        exit(1);
    }

    if (idxa == idxb) {
        fprintf(stderr, "TEST ERROR: SORT ERROR!!! (A, B)===(%d, %d)\n", idxa, idxb);
        exit(1);
    }

    return idxa - idxb;
}

TestSuite const* suite;
TestOpts opts;

// Determines how options are configured and resource release strategy
enum OptionStrategy {
    AUTOMATIC,
    MANUAL,
} opt_strategy = AUTOMATIC;


// Initialize options
void reset_opts() {
    opt_strategy = AUTOMATIC;
    opts = (TestOpts) {0};

    opts.included_size = tests_size;
    opts.included = malloc(sizeof(char*) * opts.included_size);

    // Sanity checks
    assert_is_not_null(opts.included);
    assert_is_null(opts.excluded);
    assert_is_null(opts.output_file);

    for (size_t i = 0; i < opts.included_size; i++) {
        opts.included[i] = tests[i].name;
    }
}

// Release resources
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
int test_create_suite_ok_0(void) {
    TEST_BEGIN;
    reset_opts();
    suite = create_test_suite(&opts);
    assert_is_not_null(suite);
    TEST_END;
}

// Test Suite with default options
int test_create_suite_ok_1(void) {
    TEST_BEGIN;
    reset_opts();

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_is_not_null(suite->tests);
    assert_equals_int(suite->n_tests, 6);
    assert_is_not_null(suite->opts);
    TEST_END;
}

// Exclude a few tests
int test_create_suite_ok_2(void) {
    TEST_BEGIN;
    opt_strategy = MANUAL;

    char* included[] = {
        "test_everything", "fail_unit_test_1",
        "fail_unit_test_1", "unit_fail_test_1"
    };

    opts.included = included;
    opts.included_size = sizeof(included) / sizeof(char*);

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_equals_int(suite->n_tests, opts.included_size);
    assert_is_not_null(suite->tests);

    int size = (int)suite->n_tests;
    char* actual[size];

    for (int i = 0; i < size; i++) {
        actual[i] = suite->tests[i]->name;
    }

    assert_equals_str_array_unordered(actual, included, size);
    TEST_END;
}

// Include a few tests
int test_create_suite_ok_3(void) {
    TEST_BEGIN;
    opt_strategy = AUTOMATIC; // Frees resources after test

    char* options[] = {"-r", "unit_test_1", "unit_test_2"};
    size_t opts_size = sizeof(options) / sizeof(char*);

    int ret = parse_test_opts(&opts, options, opts_size);
    assert_equals_int(ret, 0);

    suite = create_test_suite(&opts);
    assert_is_not_null(suite);

    assert_equals_int(suite->n_tests, opts.included_size);
    assert_is_not_null(suite->tests);

    char** included = &options[1];

    int size = (int)suite->n_tests;
    char* actual[size];

    for (int i = 0; i < size; i++) {
        actual[i] = suite->tests[i]->name;
    }

    assert_equals_str_array_unordered(actual, included, size);
    TEST_END;
}

// Ensure the test functions are correct
int test_create_suite_ok_4(void) {
    TEST_BEGIN;
    reset_opts();
    suite = create_test_suite(&opts);
    assert_is_not_null(suite);
    assert_is_not_null(suite->tests);
    assert_equals_int(suite->n_tests, tests_size);

    // Order for comparison
    qsort(suite->tests, tests_size, sizeof(Test*), test_sort_compar);

    for (size_t i = 0; i < tests_size; i++) {
        assert_equals_str(suite->tests[i]->name, tests[i].name);
        assert_equals_funcptr(suite->tests[i]->func, tests[i].func, test_func_t);
    }
    TEST_END;
}

Test curr_tests[] = {
    {.name="test_create_suite_ok_0", .func=test_create_suite_ok_0},
    {.name="test_create_suite_ok_1", .func=test_create_suite_ok_1},
    {.name="test_create_suite_ok_2", .func=test_create_suite_ok_2},
    {.name="test_create_suite_ok_3", .func=test_create_suite_ok_3},
    {.name="test_create_suite_ok_4", .func=test_create_suite_ok_4},
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
