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

int main(void)
{
    test_str_token_char();
    return ASSERTS_EXIT_CODE;
}
