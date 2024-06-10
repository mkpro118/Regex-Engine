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
    for (char c = 'a'; c <= 'z'; c++) {
        Token token = {.type = CHAR, .value = c};

        char expected[] = {'C', 'H', 'A', 'R', '(', '\'', c, '\'', ')', '\0'};
        const char* actual = str_token(&token);
        assert_equals_str(expected, actual);
    }
}

int main(void)
{
    test_str_token_char();
    return 0;
}
