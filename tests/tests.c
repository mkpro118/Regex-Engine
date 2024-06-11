#include "tests.h"
#include "asserts.h"

struct TestSuite {
    Test* tests;
    size_t n_tests;
    TestOpts* opts;
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
