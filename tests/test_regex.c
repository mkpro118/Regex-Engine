#include <string.h>

#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "regex.h"

// Test regex creation and initialization
int test_regex_create_and_init() {
    TEST_BEGIN;

    // Test regex_create
    Regex* regex = regex_create("a*b");
    assert_is_not_null(regex);
    assert_equals_int(true, regex->is_compiled);
    assert_is_not_null(regex->nfa);
    assert_equals_str(regex->pattern, "a*b");
    regex_free(regex);
    free(regex);

    // Test regex_init
    Regex regex_buf;
    int result = regex_init(&regex_buf, "c+d");
    assert_equals_int(result, 0);
    assert_equals_int(true, regex_buf.is_compiled);
    assert_is_not_null(regex_buf.nfa);
    assert_equals_str(regex_buf.pattern, "c+d");
    regex_free(&regex_buf);

    // Test with NULL pattern
    result = regex_init(&regex_buf, NULL);
    assert_equals_int(result, 0);
    assert_equals_int(false, regex_buf.is_compiled);
    assert_is_null(regex_buf.nfa);
    assert_is_null(regex_buf.pattern);

    // Test with NULL regex buffer
    result = regex_init(NULL, "e|f");
    assert_equals_int(result, -1);

    TEST_END;
}

// Test regex compilation
int test_regex_compile() {
    TEST_BEGIN;

    Regex regex_buf;
    regex_init(&regex_buf, NULL);

    // Test compiling a new pattern
    int result = regex_compile(&regex_buf, "a|b");
    assert_equals_int(result, 0);
    assert_equals_int(true, regex_buf.is_compiled);
    assert_is_not_null(regex_buf.nfa);
    assert_equals_str(regex_buf.pattern, "a|b");

    // Test recompiling with the same pattern
    result = regex_compile(&regex_buf, "a|b");
    assert_equals_int(result, 1);  // Should return 1 for already compiled

    // Test recompiling with a different pattern
    result = regex_compile(&regex_buf, "c*d+");
    assert_equals_int(result, 0);
    assert_equals_int(true, regex_buf.is_compiled);
    assert_is_not_null(regex_buf.nfa);
    assert_equals_str(regex_buf.pattern, "c*d+");

    // Test with NULL regex buffer
    result = regex_compile(NULL, "e|f");
    assert_equals_int(result, -1);

    // Test with NULL pattern
    result = regex_compile(&regex_buf, NULL);
    assert_equals_int(result, -1);

    regex_free(&regex_buf);

    TEST_END;
}

// Test regex matching
int test_regex_match() {
    TEST_BEGIN;

    Regex* regex = regex_create("a*b+c?");
    assert_is_not_null(regex);

    // Test matching strings
    assert_equals_int(true, regex_match(regex, "b"));
    assert_equals_int(true, regex_match(regex, "bc"));
    assert_equals_int(true, regex_match(regex, "ab"));
    assert_equals_int(true, regex_match(regex, "aabbc"));
    assert_equals_int(true, regex_match(regex, "aaaabbbbc"));

    // Test non-matching strings
    assert_equals_int(false, regex_match(regex, ""));
    assert_equals_int(false, regex_match(regex, "a"));
    assert_equals_int(false, regex_match(regex, "c"));
    assert_equals_int(false, regex_match(regex, "ac"));
    assert_equals_int(false, regex_match(regex, "bca"));

    // Test with NULL regex
    assert_equals_int(false, regex_match(NULL, "abc"));

    // Test with NULL string
    assert_equals_int(false, regex_match(regex, NULL));

    regex_free(regex);
    free(regex);

    // Test with uncompiled regex
    Regex uncompiled_regex;
    regex_init(&uncompiled_regex, NULL);
    assert_equals_int(false, regex_match(&uncompiled_regex, "abc"));

    TEST_END;
}

// Test regex freeing
int test_regex_free() {
    TEST_BEGIN;

    Regex* regex = regex_create("a|b");
    assert_is_not_null(regex);
    assert_is_not_null(regex->nfa);

    regex_free(regex);
    free(regex);

    // Test freeing NULL regex
    regex_free(NULL);  // Should not crash

    TEST_END;
}

Test tests[] = {
    {.name="test_regex_create_and_init", .func=test_regex_create_and_init},
    {.name="test_regex_compile", .func=test_regex_compile},
    {.name="test_regex_match", .func=test_regex_match},
    {.name="test_regex_free", .func=test_regex_free},
    {.name=NULL},
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
