#ifndef _RUN_TEST_H_
#define _RUN_TEST_H_

#include <stdlib.h>
#include <sys/types.h>

// Function type for test functions
typedef void (*test_func)(void);

typedef struct Test {
    char* name;
    test_func func;
} Test;

typedef struct TestSuite {
    Test* tests;
    size_t n_tests;
} TestSuite;

/**
 * Creates a test suite containing a set of test functions.
 *
 * @param tests     An array of tests
 * @param n_tests   The size of the tests array
 * @param options   An array of strings representing tests to be included in
 *                  this suite. If NULL, all tests will be used in the suite
 * @param n_options The number of options in the options array
 *                  If zero, all tests will be used in the suite
 *
 * @return A pointer to a newly allocated TestSuite structure.
 *         This function will return NULL if no test is included in the suite.
 *
 * The caller is responsible for freeing the memory allocated
 * for the test suite using free_test_suite().
 */
TestSuite const* create_test_suite(Test* tests, size_t n_tests,
                             char** options, size_t n_options);

/**
 * Release the memory allocated for the given test suite
 *
 * @param suite A pointer to the suite to be freed
 */
void free_test_suite(TestSuite* suite);

#endif // _RUN_TEST_H_
