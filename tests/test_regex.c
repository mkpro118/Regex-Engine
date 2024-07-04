#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"

#include "regex.h"

int test_regex_easy(void) {
    TEST_BEGIN;



    TEST_END;
}

Test tests[] = {
    {.name="test_regex_easy", .func=test_regex_easy},
    {.name=NULL, .func=NULL},
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
