#ifndef _RUN_TEST_H_
#define _RUN_TEST_H_

#include <stdlib.h>
#include <sys/types.h>

// Function type for test functions
typedef void (*test_func_t)(void);

// Type for Individual tests
typedef struct Test Test;

// Type for a Test Suite
typedef struct TestSuite TestSuite;

// Type for Test Suite options
typedef struct TestOpts TestOpts;

/**
 * This structure represents a single test function
 *
 * Members:
 *     - name: Name of the test
 *     - func: A function pointer to the test
 */
struct Test {
    char* name;
    test_func_t func;
};

/* This variable must be defined by tests to contain all the tests in scope */
extern Test tests[];

/**
 * This structure contains information about a test suite's execution.
 *
 * Members:
 *     - fail_fast: Set to 1 to stop exection after the first failure. Defaults to 0.
 *     - parallel: Set to 1 to run all tests using multiple threads. Defaults to 0.
 *     - verbose: Control the verbosity of information during test execution.
 *                Possible values are:
 *                    0: Turn off verbosity
 *                    1: Only display progress
 *                    2: Display each test with progress report
 */
struct TestOpts {
    unsigned char fail_fast: 1;
    unsigned char parallel: 1;
    unsigned char verbose: 2;
    unsigned char randomize: 1;
    unsigned char dry_run: 1;

    unsigned int timeout;

    char** included;
    size_t included_size;

    char** ignored;
    size_t ignored_size;

    char* output_file;
};

/**
 * @return A help/usage string for the options used by a Test Suite
 */
const char* opts_help_str(void);

/**
 * Prints a help/usage string for the options used by a Test Suite to stdout
 */
void print_opts_help(void);

/**
 * Parse args to create a set of options used by the test runner.
 * Available options are:
 *   - -ff, --fail-fast: Stop running tests after the first failure.
 *   -  -v, --verbose <level>: Runner's versbosity level. 0, 1 or 2
 *   -  -r, --run <test_name>: Run only the specified test(s).
 *   -  -x, --exclude <test_name>: Exclude specific test(s) from running.
 *   -  -o, --output <file>: Redirect test results to a specified file.
 *   -  -t, --timeout <milliseconds>: Set a maximum runtime for each test.
 *   -  -d, --dry-run: Display the tests that would be run without executing them.
 *   -  -s, --summary: Print only the summary of the test results. (Sets verbosity to 0)
 *   -  -z, --randomize: Run tests in a random order.
 *
 * @param  opts_buf  [description]
 * @param  opts      [description]
 * @param  opts_size [description]
 * @return           [description]
 */
int parse_test_opts(TestOpts* opts_buf, char** opts, size_t opts_size);

/**
 * Release internally allocated memory by the options structure
 *
 * NOTE: This function does not release `opts` itself, as it may be created
 * statically. Memory for `opts` must be appropriately managed by its owner.
 *
 * @param opts A pointer to the options structure to be freed
 */
void free_opts(TestOpts* opts);

/**
 * Creates a test suite containing a set of test functions.
 *
 * @param opts Options used for the current test suite
 *
 * @return A pointer to a newly allocated TestSuite structure.
 *         This function will return NULL if no test is included in the suite.
 *
 * The caller is responsible for freeing the memory allocated
 * for the test suite using free_test_suite().
 */
TestSuite const* create_test_suite(TestOpts* opts);

/**
 * Release the memory allocated for the given test suite
 *
 * @param suite A pointer to the suite to be freed
 */
void free_test_suite(TestSuite* suite);

/**
 * Runs the set of tests specified by the current test suite.
 *
 * @param  suite The test suite to run
 *
 * @return Number of tests failed, 0 if all tests succeed
 */
int run_test_suite(TestSuite* suite);

#endif // _RUN_TEST_H_
