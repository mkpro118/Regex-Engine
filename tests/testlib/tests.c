#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "asserts.h"
#include "portability.h"

#define STRTOL_BASE_OR_RADIX 10

#define NO_ERROR 0
#define UNSUPPORTED_OPTION 1
#define STRTOL_ERROR 2
#define NO_VALUE_TO_VERBOSE 3
#define INVALID_VALUE_TO_VERBOSE 4
#define NO_VALUE_TO_RUN 5
#define UNKNOWN_TEST_TO_RUN 6
#define NO_VALUE_TO_EXCLUDE 7
#define UNKNOWN_TEST_TO_EXCLUDE 8
#define NO_VALUE_TO_OUTPUT_FILE 9
#define NO_VALUE_TO_TIMEOUT 10
#define INVALID_VALUE_TO_TIMEOUT 11
#define SUMMARY_AND_VERBOSE 12
#define ALLOCATOR_FAILED 13

enum TestResult {
    ABORTED,
    FAILURE,
    SKIPPED,
    SUCCESS,
    TIMEOUT,
};

char* TEST_RESULT_STR[] = {
    "Aborted",
    "Failed",
    "Skipped",
    "Passed",
    "Timed out",
};

size_t const N_RESULT_VARIANTS = sizeof(TEST_RESULT_STR) / sizeof(char*);

typedef struct TestThreadArgs {
    TestSuite* suite;
    size_t idx;
    enum TestResult result;
} TestThreadArgs;

typedef struct TestResults {
    Test* test;
    enum TestResult result;
} TestResults;

typedef struct TestSuiteResults {
    TestSuite* suite;
    TestResults* results;
} TestSuiteResults;

// The default options for any test suite
static const TestOpts DEFAULT_OPTS = {
    .fail_fast = 0,
    .parallel = 0,
    .verbose = 1,
    .randomize = 0,
    .dry_run = 0,
    .timeout = 0,
    .summary = 0,
    .included = NULL,
    .included_size = 0,
    .excluded = NULL,
    .excluded_size = 0,
    .output_file = NULL,
};

const char tests_help_str[] =
"Parse args to create a set of options used by the test runner.\n"
"Available options are:\n"
"  -ff, --fail-fast: Stop running tests after the first failure.\n"
"  -v, --verbose <level>: Runner's versbosity level. 0, 1 or 2\n"
"  -r, --run <test_name>: Run only the specified test(s).\n"
"  -x, --exclude <test_name>: Exclude specific test(s) from running.\n"
"  -o, --output <file>: Redirect test results to a specified file.\n"
"  -t, --timeout <milliseconds>: Set a maximum runtime for each test.\n"
"  -d, --dry-run: Display the tests that would be run without executing them.\n"
"  -s, --summary: Print only the summary of the test results. (Sets verbosity "
"to 0)\n"
"  -z, --randomize: Run tests in a random order.\n";


// A help/usage string for the options used by a Test Suite
const char* opts_help_str(void) {
    return tests_help_str;
}


// Prints a help/usage string for the options used by a Test Suite to stdout
void print_opts_help(void) {
    fprintf(stdout, "%s", tests_help_str);
}


// Returns a message given a parser error code
const char* str_parse_error(int err) {
    switch (err) {
    case NO_ERROR:
        return "no error";
    case UNSUPPORTED_OPTION:
        return "unsupported option passed";
    case STRTOL_ERROR:
        return "error on call to strtol. check errno for more information";
    case NO_VALUE_TO_VERBOSE:
        return "no value provided to the verbose option";
    case INVALID_VALUE_TO_VERBOSE:
        return "invalid value provided to the verbose option";
    case NO_VALUE_TO_RUN:
        return "no value provided to the run option";
    case UNKNOWN_TEST_TO_RUN:
        return "unknown test specified to run";
    case NO_VALUE_TO_EXCLUDE:
        return "no value provided to the exclude option";
    case UNKNOWN_TEST_TO_EXCLUDE:
        return "unknown test specified to exclude";
    case NO_VALUE_TO_OUTPUT_FILE:
        return "no value provided to the output file option";
    case NO_VALUE_TO_TIMEOUT:
        return "no value provided to the timeout option";
    case INVALID_VALUE_TO_TIMEOUT:
        return "invalid value provided to the timeout option";
    case SUMMARY_AND_VERBOSE:
        return "--summary and --verbose cannot be specified together";
    case ALLOCATOR_FAILED:
        return "memory allocator failed. check errno for more information";
    default:
        errno = EINVAL;
        return "unknown error";
    }
}


// Find a test given it's name
Test* find_test_by_name(char* name) {
    Test* test_ptr = tests;
    while (test_ptr->name != NULL) {
        if (strcmp(name, test_ptr->name) == 0)
            return test_ptr;
        test_ptr++;
    }
    return NULL;
}


// Returns 1 if arr conatains val, 0 otherwise
static inline int _contains(char** arr, size_t size, char* val) {
    for (size_t i = 0; i < size; i++) {
        if (strcmp(val, arr[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


// Add the test with the given name to the options
// This internal function assumes opts_buf->included array is initialized
// and has capacity for at least one more element
static inline int include_test(TestOpts* opts_buf, char* name) {
    // If `name` is repeated, do not add it again
    if (_contains(opts_buf->included, opts_buf->included_size, name)) {
        return 0;
    }

    Test* test_ptr;

    // Ensure the next value is a recognized test function
    if ((test_ptr = find_test_by_name(name)) == NULL) {
        return UNKNOWN_TEST_TO_RUN;
    }

    // If it is, add it to the included tests
    opts_buf->included[opts_buf->included_size++] = test_ptr->name;

    return 0; // Success
}


// Add the test with the given name to the excluded tests
// This internal function assumes opts_buf->excluded array is initialized
// and has capacity for at least one more element
static inline int exclude_test(TestOpts* opts_buf, char* name) {
    // If `name` is repeated, do not add it again
    if (_contains(opts_buf->excluded, opts_buf->excluded_size, name)) {
        return 0;
    }

    Test* test_ptr;
    // Ensure the next value is a recognized test function
    if ((test_ptr = find_test_by_name(name)) == NULL) {
        return UNKNOWN_TEST_TO_EXCLUDE;
    }

    // If it is, add it to the excluded tests
    opts_buf->excluded[opts_buf->excluded_size++] = test_ptr->name;

    return 0; // Success
}


// Adds all the tests to the suite assuming no test is excluded
// Assumes enough memory has been allocated appropriately
// This is an optimized version of `include_selective`, which adds checks to
// filter excluded tests
int include_all_tests(TestOpts* opts_buf) {
    Test* test_ptr = tests;
    size_t idx = 0;
    while (test_ptr->name != NULL) {
        opts_buf->included[idx++] = test_ptr->name;
        test_ptr++;
    }
    return idx;
}


// Adds all non-excluded tests.
// Assumes enough memory has been allocated appropriately
int include_selective(TestOpts* opts_buf) {
    Test* test_ptr = tests;
    size_t idx = 0;
    while (test_ptr->name != NULL) {
        if (!_contains(opts_buf->excluded, opts_buf->excluded_size, test_ptr->name)) {
            opts_buf->included[idx++] = test_ptr->name;
        }
        test_ptr++;
    }
    return idx;
}

// Macro for option string comparison
#define check_opt(var, shorthand, option) (strcmp(var, shorthand) == 0 || strcmp(var, option) == 0)

// Macro to release resources on failure and return a failure code
#define free_opts_and_return(opt_ptr, exit_code) {\
    free_opts((opt_ptr));\
    return (exit_code);\
}

// Parse args to create a set of options used by the test runner.
int parse_test_opts(TestOpts* opts_buf, char** opts, size_t opts_size) {
    // Clear out old options
    *opts_buf = DEFAULT_OPTS;

    // Determine the total number of tests
    Test* test_ptr = tests;
    size_t n_tests = 0;
    while (test_ptr->name != NULL) {
        n_tests++;
        test_ptr++;
    }

    // Initially, assume all tests could be included or excluded
    opts_buf->included = malloc(sizeof(char*) * n_tests);
    if (opts_buf->included == NULL) {
        free_opts_and_return(opts_buf, ALLOCATOR_FAILED);
    }

    // In case options are not provided, use defaults and include all tests
    if (NULL == opts || 0 == opts_size) {
        opts_buf->included_size = include_all_tests(opts_buf);
        return NO_ERROR;
    }

    opts_buf->excluded = malloc(sizeof(char*) * n_tests);
    if (opts_buf->excluded == NULL) {
        free_opts_and_return(opts_buf, ALLOCATOR_FAILED);
    }

    // If the run option is not provided,
    // execute all tests that are not excluded
    unsigned char opt_run_seen = 0;

    enum parse_contexts {
        NO_CONTEXT,
        INCLUDE,
        EXCLUDE,
    } context = NO_CONTEXT;

    for (size_t i = 0; i < opts_size; i++) {
        char* opt = opts[i];

        /* Boolean options */
        // Fail Fast
        if (check_opt(opt, "-ff", "--fail-fast")) {
            context = NO_CONTEXT;
            opts_buf->fail_fast = 1;
        }
        // Parallel execution
        else if (check_opt(opt, "-p", "--parallel")) {
            context = NO_CONTEXT;
            opts_buf->parallel = 1;
        }
        // Dry Run
        else if (check_opt(opt, "-d", "--dry-run")) {
            context = NO_CONTEXT;
            opts_buf->dry_run = 1;
        }
        // Summary only
        else if (check_opt(opt, "-s", "--summary")) {
            context = NO_CONTEXT;
            opts_buf->summary = 1;
            opts_buf->verbose = 0;
        }
        // Randomize
        else if (check_opt(opt, "-z", "--randomize")) {
            context = NO_CONTEXT;
            opts_buf->randomize = 1;
        }

        /* Numeric options */

        // Verbosity
        else if (check_opt(opt, "-v", "--verbose")) {
            context = NO_CONTEXT;

            if (opts_buf->summary == 1) {
                free_opts_and_return(opts_buf, SUMMARY_AND_VERBOSE);
            }

            if (++i >= opts_size) {
                free_opts_and_return(opts_buf, NO_VALUE_TO_VERBOSE);
            }

            char* next = opts[i]; // The next value in the options string
            char* end; // For strtol

            errno = 0;
            long val = strtol(next, &end, STRTOL_BASE_OR_RADIX);

            // If an error occured with strtol
            if (errno != 0) {
                free_opts_and_return(opts_buf, STRTOL_ERROR);
            }

            // No digits were parsed
            if (end == next) {
                free_opts_and_return(opts_buf, INVALID_VALUE_TO_VERBOSE);
            }

            // Check value is valid
            if (val < 0 || val > 2) {
                free_opts_and_return(opts_buf, INVALID_VALUE_TO_VERBOSE);
            }

            opts_buf->verbose = val;
        }
        // Timeout
        else if (check_opt(opt, "-t", "--timeout")) {
            context = NO_CONTEXT;

            if (++i >= opts_size) {
                free_opts_and_return(opts_buf, NO_VALUE_TO_TIMEOUT);
            }

            char* next = opts[i]; // The next value in the options string
            char* end; // For strtol

            errno = 0;
            long val = strtol(next, &end, STRTOL_BASE_OR_RADIX);

            // If an error occured with strtol
            if (errno != 0) {
                free_opts_and_return(opts_buf, STRTOL_ERROR);
            }

            // No digits were parsed
            if (end == next) {
                free_opts_and_return(opts_buf, INVALID_VALUE_TO_TIMEOUT);
            }

            // Ensure timeout is not negative
            if (val < 0) {
                free_opts_and_return(opts_buf, INVALID_VALUE_TO_TIMEOUT);
            }

            // All good
            opts_buf->timeout = val;
        }

        /* String options */

        // Run the following tests
        else if (check_opt(opt, "-r", "--run")) {
            opt_run_seen = 1;
            context = INCLUDE;

            if (++i >= opts_size) {
                free_opts_and_return(opts_buf, NO_VALUE_TO_RUN);
            }

            int ret = include_test(opts_buf, opts[i]);
            // If an error occured, exit
            if (ret != 0) {
                free_opts_and_return(opts_buf, ret);
            }
        }
        // Ignore the following tests
        else if (check_opt(opt, "-x", "--exclude")) {
            context = EXCLUDE;

            if (++i >= opts_size) {
                free_opts_and_return(opts_buf, NO_VALUE_TO_EXCLUDE);
            }

            int ret = exclude_test(opts_buf, opts[i]);
            // If an error occured, exit
            if (ret != 0) {
                free_opts_and_return(opts_buf, ret);
            }
        }
        // Output file
        else if (check_opt(opt, "-o", "--output")) {
            context = NO_CONTEXT;

            if (++i >= opts_size) {
                free_opts_and_return(opts_buf, NO_VALUE_TO_OUTPUT_FILE);
            }

            // We will make a copy of this
            // as it may or may not be in static memory
            opts_buf->output_file = strdup(opts[i]);
        } else {
            int ret = 0; // Return value of include_test/exclude_test

            switch (context) {
            case INCLUDE:
                ret = include_test(opts_buf, opt);
                break;
            case EXCLUDE:
                ret = exclude_test(opts_buf, opt);
                break;
            case NO_CONTEXT:
                free_opts_and_return(opts_buf, UNSUPPORTED_OPTION);
            }

            // If include/exclude fails, exit
            if (ret != 0) {
                free_opts_and_return(opts_buf,  ret);
            }
        }
    }

    // If the --run option was not explicity specified, include all tests except
    // the excluded ones
    if (opt_run_seen == 0) {
        if (opts_buf->excluded_size == 0) {
            opts_buf->included_size = include_all_tests(opts_buf);
        } else {
            opts_buf->included_size = include_selective(opts_buf);
        }
    }

    // Tight layout
    char** temp;

    // Resize memory allocated for included tests
    if (opts_buf->included_size == 0) {
        free(opts_buf->included);
        opts_buf->included = NULL;
    } else {
        temp = realloc(opts_buf->included, sizeof(char*) * opts_buf->included_size);
        if (temp == NULL) {
            free_opts_and_return(opts_buf, ALLOCATOR_FAILED);
        }
        opts_buf->included = temp;
    }

    // Resize memory allocated for excluded tests
    if (opts_buf->excluded_size == 0) {
        free(opts_buf->excluded);
        opts_buf->excluded = NULL;
    } else {
        temp = realloc(opts_buf->excluded, sizeof(char*) * opts_buf->excluded_size);
        if (temp == NULL) {
            free_opts_and_return(opts_buf, ALLOCATOR_FAILED);
        }
        opts_buf->excluded = temp;
    }

    return NO_ERROR;
}


// Release internally allocated memory by the options structure
void free_opts(TestOpts* opts) {
    if (opts == NULL) {
        return;
    }

    free(opts->included);
    free(opts->excluded);
    free(opts->output_file);

    *opts = DEFAULT_OPTS;
}


// Macro to release resources on failure and return a failure code
#define free_suite_and_return(suite_ptr) {\
    free_test_suite((suite_ptr));\
    return NULL;\
}


// Creates a test suite containing a set of test functions
// Returns NULL if no tests are included
TestSuite const* create_test_suite(TestOpts* opts) {
    TestSuite* suite = NULL;

    // Cannot create a test suite with no included tests
    if (opts->included_size == 0 || opts->included == NULL) {
        return NULL;
    }

    // If allocation fails, return NULL
    if ((suite = malloc(sizeof(TestSuite))) == NULL) {
        return NULL;
    }

    // Initialize number of tests to 0, may have garbage value after malloc
    suite->n_tests = 0;

    // Allocate memory for the array of tests in the suite
    suite->tests = malloc(sizeof(Test*) * opts->included_size);
    if (suite->tests == NULL) {
        free_suite_and_return(suite);
    }

    // Include tests into the suite
    for (size_t i = 0; i < opts->included_size; i++) {
        Test* test_ptr = find_test_by_name(opts->included[i]);

        // If an included test is not found, this is an error.
        // This should ideally be caught by the options parser, but if
        // opts was created manually, this will prevent further errors
        if (test_ptr == NULL) {
            free_suite_and_return(suite);
        }

        suite->tests[suite->n_tests++] = test_ptr;
    }

    suite->opts = opts;

    return suite;
}


// Release the memory allocated for the given test suite
// Does NOT deallocate the associated TestOpts
void free_test_suite(TestSuite const* suite) {
    if (suite == NULL) {
        return;
    }

    // Release memory allocated for Tests
    free(suite->tests);

    // Release memory allocated for the TestSuite structure
    free((void*) suite);
}


// Display dry run information
// Assumes suite is a well formed object, and handle is valid
void dry_run(TestSuite* suite, FILE* handle) {
    fprintf(handle, "\nDry-Run Report\n--------------\n");
    // Display relevant options
    fprintf(handle, "The following strategies will be used:\n");
    // Parallel or Sequential
    if (suite->opts->parallel) {
        fprintf(handle, " - Tests will be executed in parallel using upto %d threads\n", 0);
    } else {
        fprintf(handle, " - Tests will be executed sequentially\n");
    }

    // Fail Fast
    if (suite->opts->fail_fast) {
        fprintf(handle, " - Tests will stop after the first failure\n");
    }

    // Randomization
    if (suite->opts->randomize) {
        fprintf(handle, " - Tests will execute in random order\n");
    }

    // Timeout
    if (suite->opts->timeout > 0) {
        fprintf(handle, " - Time limit per tests is %dms\n", suite->opts->timeout);
    } else {
        fprintf(handle, " - No time limit is set\n");
    }

    // Verbosity
    if (suite->opts->verbose > 0) {
        fprintf(handle, " - Versbosity level %d\n", suite->opts->verbose);
    } else {
        fprintf(handle, " - Versbosity is turned off\n");
    }

    size_t size = suite->opts->included_size;
    if (size == 0) {
        fprintf(handle, "\nThere are no tests to be run in this suite.\n");
        return;
    } else {
        fprintf(handle, "\nThe following tests will be run (%d):\n", (int) size);

        for (size_t i = 0; i < size; i++) {
            fprintf(handle, " %s\n", suite->opts->included[i]);
        }
    }

    // Excluded tests, if any
    size = suite->opts->excluded_size;
    if (size == 0) {
        return;
    }

    fprintf(handle, "\nThe following tests will not be run (%d):\n", (int) size);

    for (size_t i = 0; i < size; i++) {
        fprintf(handle, " %s\n", suite->opts->excluded[i]);
    }
}

void run_test(void* args) {
    TestThreadArgs* test = (TestThreadArgs*) args;

    int exit_code = test->suite->tests[test->idx]->func();

    if (exit_code != 0) {
        test->result = FAILURE;
    } else {
        test->result = SUCCESS;
    }
}

// Runs all included tests in parallel
// Assumes suite is a well formed object, and handle is valid
TestSuiteResults run_tests_parallel(TestSuite const* suite, FILE* handle) {
    fprintf(stdout, "lol %p, %p\n", (void*)suite, (void*)handle);
    return (TestSuiteResults) {0};
}

// Runs all included tests sequentially
// Assumes suite is a well formed object, and handle is valid
TestSuiteResults run_tests_sequential(TestSuite const* suite, FILE* handle) {
    TestResults* results = malloc(sizeof(TestResults) * suite->n_tests);
    if (results == NULL) {
        fprintf(handle, "Critcal Error. errno: %d strerror: %s\n", errno, strerror(errno));
        exit(1);
    }

    TestSuiteResults suite_results = {
        .suite = (TestSuite*) suite,
        .results = results,
    };

    TestThreadArgs args = {
        .suite = suite_results.suite,
        .idx = 0,
        .result = SKIPPED,
    };

    int verbosity = suite->opts->verbose;
    if (verbosity > 0) {
        fprintf(handle, "\n");
    }

    for (size_t i = 0; i < suite->n_tests; i++) {
        args.idx = i;
        run_test(&args);
        results[i] = (TestResults) {
            .test = suite->tests[i],
            .result = args.result,
        };

        if (verbosity == 1) {
            char indicator;

            switch (args.result) {
            case ABORTED:
                indicator = 'A';
                break;
            case FAILURE:
                indicator = 'F';
                break;
            case SKIPPED:
                indicator = 'S';
                break;
            case SUCCESS:
                indicator = '.';
                break;
            case TIMEOUT:
                indicator = 'T';
                break;
            default:
                fprintf(handle, "\nCritical runtime error. errno: %d, strerror: %s\n",
                        errno, strerror(errno));
                exit(1);
            }

            fprintf(handle, "%c", indicator);
            fflush(handle);
        }
    }

    if (verbosity > 0) {
        fprintf(handle, "\n");
    }

    return suite_results;
}

// Runs the set of tests specified by the current test suite
int run_test_suite(TestSuite const* test_suite) {
    // Assume that options were given
    unsigned char opts_missing = 0;
    TestSuite suite = *test_suite;
    int n_failures = 0;

    if (suite.opts == NULL) { // If options were not provided, use defaults
        opts_missing = 1; // options were not given
        suite.opts = malloc(sizeof(TestOpts));
        parse_test_opts(suite.opts, NULL, 0);
    }

    FILE* handle = stdout; // Default to using stdout
    if (suite.opts->output_file != NULL) {
        handle = fopen(suite.opts->output_file, "w");
    }

    if (suite.opts->dry_run) {
        dry_run(&suite, handle);
        return 0;
    }

    // These run_test_* function perform expensive setup operations
    // Only run them if there actually are tests to run
    if (suite.n_tests > 0) {
        TestSuiteResults suite_results;
        if (suite.opts->parallel) {
            suite_results = run_tests_parallel(&suite, handle);
        } else {
            suite_results = run_tests_sequential(&suite, handle);
        }

        int stats[N_RESULT_VARIANTS];
        memset(stats, 0, sizeof(stats));

        // Count each type of result
        for (size_t i = 0; i < suite.n_tests; i++) {
            stats[suite_results.results[i].result]++;
        }

        // Everything that didn't succeed or wasn't skipped is a failure
        n_failures = suite.n_tests - (stats[SUCCESS] + stats[SKIPPED]);

        for (size_t i = 0; i < N_RESULT_VARIANTS; i++) {
            fprintf(handle, "%s: %d\n", TEST_RESULT_STR[i], stats[i]);
        }

        free(suite_results.results);
    }

    // If options were not given, we must have allocated them
    // Release those resources now to prevent memory leaks
    if (opts_missing) {
        free_opts(suite.opts); // This doesn't free the opts itself
        free(suite.opts); // So free it here
        suite.opts = NULL; // Reset memory
    }

    return n_failures;
}

// Default entrypoint for test files
int default_main(char** argv, size_t argc) {
    TestOpts opts;
    int ret = parse_test_opts(&opts, argv, argc);
    if (ret != 0) {
        fprintf(stderr, "%s\n", str_parse_error(ret));
        exit(1);
    }

    const TestSuite* suite = create_test_suite(&opts);

    int n_failures = run_test_suite(suite);

    free_opts(&opts);
    free_test_suite(suite);

    return n_failures;
}
