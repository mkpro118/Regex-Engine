#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "token.h"
#include "lexer.h"

#define _TOKEN_TESTS_
#define _LEXER_TESTS_
#include "asserts.h"

// Tests creation of a dynamically allocated lexer
int test_lexer_create(void) {
    TEST_BEGIN;
    char regex[] = "abc|def";
    Lexer* lexer = lexer_create(regex);
    assert_is_not_null(lexer);

    Lexer exp = {
        ._regex = regex,
        ._position = 0,
        ._regex_len = strlen(regex)
    };

    assert_equals_lexer(lexer, &exp);

    lexer_free(lexer);
    free(lexer);
    TEST_END;
}

// Tests the initialized of a given lexer.
int test_lexer_init(void) {
    TEST_BEGIN;
    Lexer lexer;
    char regex[] = "a|b*";

    int ret = lexer_init(&lexer, regex);
    assert_equals_int(ret, 0);

    Lexer exp = {
        ._regex = regex,
        ._position = 0,
        ._regex_len = strlen(regex)
    };

    assert_equals_lexer(&lexer, &exp);

    lexer_free(&lexer);
    TEST_END;
}

// Test that the lexer returns the correct token
int test_next_token(void) {
    TEST_BEGIN;
    char regex[] = "a+|b*";

    Lexer lexer;
    int ret = lexer_init(&lexer, regex);
    assert_equals_int(ret, 0);

    Token token;
    Token* token_ptr = &token;

    // Expected tokens
    Token exp[] = {
        {.type=CHAR, .value='a'},
        {.type=PLUS},
        {.type=OR},
        {.type=CHAR, .value='b'},
        {.type=STAR},
    };

    size_t n_tokens = sizeof(exp) / sizeof(Token);

    for (Token* exp_ptr = exp; exp_ptr < &exp[n_tokens]; exp_ptr++) {
        ret = next_token(&lexer, &token);
        assert_equals_int(ret, 0);
        assert_equals_token(exp_ptr, token_ptr);
    }

    lexer_free(&lexer);
    TEST_END;
}

// Test that the lexer correctly tokenizes a given number of tokens
// in the input
int test_tokenize(void) {
    TEST_BEGIN;
    char regex[] = "(a|b)*((def|gh))";

    Lexer lexer;
    int ret = lexer_init(&lexer, regex);
    assert_equals_int(ret, 0);

    const int buf_size = 5;
    Token token_buf[buf_size];

    ret = tokenize(&lexer, token_buf, buf_size);
    assert_equals_int(ret, buf_size);

    // Expected tokens
    Token exp[] = {
        {.type=LPAREN},
        {.type=CHAR, .value='a'},
        {.type=OR},
        {.type=CHAR, .value='b'},
        {.type=RPAREN},
    };

    for (int i = 0; i < buf_size; i++) {
        assert_equals_token((&exp[i]), (&token_buf[i]));
    }

    lexer_free(&lexer);
    TEST_END;
}

// Test that the lexer correctly tokenizes all of the tokens in the input 
int test_tokenize_all(void) {
    TEST_BEGIN;
    char regex[] = "(a|b)*((def|gh))";

    Lexer lexer;
    int ret = lexer_init(&lexer, regex);
    assert_equals_int(ret, 0);

    size_t size = 0;
    Token* tokens = tokenize_all(&lexer, &size);
    assert_is_not_null(tokens);
    assert_equals_int(size, strlen(regex));

    // Expected tokens
    Token exp[] = {
        {.type=LPAREN}, {.type=CHAR, .value='a'}, {.type=OR},
        {.type=CHAR, .value='b'}, {.type=RPAREN},{.type=STAR},
        {.type=LPAREN}, {.type=LPAREN}, {.type=CHAR, .value='d'},
        {.type=CHAR, .value='e'}, {.type=CHAR, .value='f'},
        {.type=OR}, {.type=CHAR, .value='g'}, {.type=CHAR, .value='h'},
        {.type=RPAREN}, {.type=RPAREN},
    };

    for (size_t i = 0; i < size; i++) {
        assert_equals_token((&exp[i]), (&tokens[i]));
    }

    lexer_free(&lexer);
    free(tokens);
    TEST_END;
}

Test tests[] = {
    {.name="test_lexer_create", .func=test_lexer_create},
    {.name="test_lexer_init", .func=test_lexer_init},
    {.name="test_next_token", .func=test_next_token},
    {.name="test_tokenize", .func=test_tokenize},
    {.name="test_tokenize_all", .func=test_tokenize_all},
    {.name=NULL},
};

int main(int argc, char* argv[]) {
    TestOpts opts;
    int ret = parse_test_opts(&opts, &argv[1], argc - 1);
    if (ret != 0) {
        printf("%s\n", str_parse_error(ret));
        exit(1);
    }

    const TestSuite* suite = create_test_suite(&opts);

    int n_failures = run_test_suite(suite);

    free_opts(&opts);
    free_test_suite(suite);

    return n_failures;
}
