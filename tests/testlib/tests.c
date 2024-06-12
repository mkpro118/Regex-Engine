#include <errno.h>

#include "tests.h"
#include "asserts.h"

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

struct TestSuite {
    Test* tests;
    size_t n_tests;
    TestOpts* opts;
};

static const TestOpts defaultOpts = {
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

    // Ensure the next value is a recognized test function
    if (find_test_by_name(name) == NULL) {
        return UNKNOWN_TEST_TO_RUN;
    }

    // If it is, add it to the included tests
    opts_buf->included[opts_buf->included_size++] = name;

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

    // Ensure the next value is a recognized test function
    if (find_test_by_name(name) == NULL) {
        return UNKNOWN_TEST_TO_EXCLUDE;
    }

    // If it is, add it to the included tests
    opts_buf->excluded[opts_buf->excluded_size++] = name;

    return 0; // Success
}


// Macro for option string comparison
#define check_opt(var, shorthand, option) (strcmp(var, shorthand) == 0 || strcmp(var, option) == 0)


// Parse args to create a set of options used by the test runner.
int parse_test_opts(TestOpts* opts_buf, char** opts, size_t opts_size) {
    // Clear out old options
    *opts_buf = defaultOpts;

    if (NULL == opts || 0 == opts_size) {
        return NO_ERROR;
    }

    // Determine the total number of tests
    Test* test_ptr = tests;
    size_t n_tests = 0;
    while (test_ptr->name != NULL) {
        n_tests++;
        test_ptr++;
    }

    // Initially, assume all tests could be included or excluded
    opts_buf->included = malloc(sizeof(char*) * n_tests);
    opts_buf->included_size = 0;
    opts_buf->excluded = malloc(sizeof(char*) * n_tests);
    opts_buf->excluded_size = 0;

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
                return SUMMARY_AND_VERBOSE;
            }

            if (++i >= opts_size) {
                return NO_VALUE_TO_VERBOSE;
            }

            char* next = opts[i]; // The next value in the options string
            char* end; // For strtol

            errno = 0;
            long val = strtol(next, &end, STRTOL_BASE_OR_RADIX);

            // If an error occured with strtol
            if (errno != 0) {
                return STRTOL_ERROR;
            }

            // No digits were parsed
            if (end == next) {
                return INVALID_VALUE_TO_VERBOSE;
            }

            // Check value is valid
            if (val < 0 || val > 2) {
                return INVALID_VALUE_TO_VERBOSE;
            }

            opts_buf->verbose = val;
        }
        // Timeout
        else if (check_opt(opt, "-t", "--timeout")) {
            context = NO_CONTEXT;

            if (++i >= opts_size) {
                return NO_VALUE_TO_TIMEOUT;
            }

            char* next = opts[i]; // The next value in the options string
            char* end; // For strtol

            errno = 0;
            long val = strtol(next, &end, STRTOL_BASE_OR_RADIX);

            // If an error occured with strtol
            if (errno != 0) {
                return STRTOL_ERROR;
            }

            // No digits were parsed
            if (end == next) {
                return INVALID_VALUE_TO_TIMEOUT;
            }

            // Ensure timeout is not negative
            if (val < 0) {
                return INVALID_VALUE_TO_TIMEOUT;
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
                return NO_VALUE_TO_RUN;
            }

            int ret = include_test(opts_buf, opts[i]);
            // If an error occured, exit
            if (ret != 0) {
                return ret;
            }
        }
        // Ignore the following tests
        else if (check_opt(opt, "-x", "--exclude")) {
            context = EXCLUDE;

            if (++i >= opts_size) {
                return NO_VALUE_TO_EXCLUDE;
            }

            int ret = exclude_test(opts_buf, opts[i]);
            // If an error occured, exit
            if (ret != 0) {
                return ret;
            }
        }
        // Output file
        else if (check_opt(opt, "-o", "--output")) {
            context = NO_CONTEXT;

            if (++i >= opts_size) {
                return NO_VALUE_TO_OUTPUT_FILE;
            }
            opts_buf->output_file = opts[i];
        } else {
            int ret; // Return value of include_test/exclude_test

            switch (context) {
            case INCLUDE:
                ret = include_test(opts_buf, opt);
                break;
            case EXCLUDE:
                ret = exclude_test(opts_buf, opt);
                break;
            case NO_CONTEXT:
                return UNSUPPORTED_OPTION;
            }

            // If include/exclude fails, exit
            if (ret != 0) {
                return ret;
            }
        }
    }

    return NO_ERROR;
}
