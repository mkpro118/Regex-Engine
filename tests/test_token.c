#include <ctype.h>

#include "asserts.h"

// CHAR,
// LPAREN,
// OR,
// PLUS,
// QUESTION,
// RPAREN,
// STAR,
// EOF,
// ERROR,

void test_str_token_char(void) {
    for (unsigned char c = 0; c <= 127; c++) {
        // We do not try non printable characters
        if (!isprint(c)) {
            continue;
        }

        Token token = {.type = CHAR, .value = c};

        char expected[] = {'C', 'H', 'A', 'R', '(', '\'', c, '\'', ')', '\0'};
        const char* actual = str_token(&token);
        assert_equals_str(expected, actual);
    }
}

#define test_str_token_for(for_, type_, expected_) \
void test_str_token_##for_(void) {\
    Token token = {.type = (type_)};\
    const char expected[] = expected_;\
    const char* actual = str_token(&token);\
    assert_equals_str(expected, actual);\
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
test_str_token_for(star, STAR, "STaR")

int main(void) {
    test_str_token_char();
    test_str_token_lparen();
    test_str_token_or();
    test_str_token_plus();
    test_str_token_question();
    test_str_token_rparen();
    test_str_token_star();
    return ASSERTS_EXIT_CODE;
}
