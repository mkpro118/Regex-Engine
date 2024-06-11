#include "tests.h"

struct TestSuite {
    Test* tests;
    size_t n_tests;
    TestOpts* opts;
};
