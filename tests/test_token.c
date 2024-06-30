#include <ctype.h>

#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "token.h"

int test_str_token_char(void) {
    TEST_BEGIN;
    for (unsigned char c = 32; c <= 127; c++) {
        // We do not try non printable characters
        if (!isprint(c)) {
            continue;
        }

        Token token = {.type = CHAR, .value = c};

        char expected[] = {'C', 'H', 'A', 'R', '(', '\'', c, '\'', ')', '\0'};
        const char* actual = str_token(&token);
        assert_equals_str(expected, actual);


    }
    TEST_END;
}

#define test_str_token_for(for_, type_, expected_) \
int test_str_token_##for_(void) {\
    TEST_BEGIN;\
    Token token = {.type = (type_)};\
    const char expected[] = expected_;\
    const char* actual = str_token(&token);\
    assert_equals_str(expected, actual);\
    TEST_END;\
}

// Left Parentheses ('(')
test_str_token_for(lparen, LPAREN, "LPAREN")

// Or/alternation ('|')
test_str_token_for(or, OR, "OR")

// Plus/Kleene Plus ('+')
test_str_token_for(plus, PLUS, "PLUS")

// Question mark ('?')
test_str_token_for(question, QUESTION, "QUESTION")

// Right Parentheses (')')
test_str_token_for(rparen, RPAREN, "RPAREN")

// Star/Kleene Star ('+')
test_str_token_for(star, STAR, "STAR")

Test tests[] = {
    {.name="test_str_token_char", .func=test_str_token_char},
    {.name="test_str_token_lparen", .func=test_str_token_lparen},
    {.name="test_str_token_or", .func=test_str_token_or},
    {.name="test_str_token_plus", .func=test_str_token_plus},
    {.name="test_str_token_question", .func=test_str_token_question},
    {.name="test_str_token_rparen", .func=test_str_token_rparen},
    {.name="test_str_token_star", .func=test_str_token_star},
    {.name=NULL},
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
