#ifndef _REGEX_ASSERTS_H_
#define _REGEX_ASSERTS_H_

#ifdef _TOKEN_TESTS_

#include "token.h"

/* Compare tokens, and display a message about the failure */
#define assert_equals_token(a, b) do {\
    mem_equals((a), (b), Token*);\
    if (a->type != b->type) {\
        ASSERTION_FAILED("Token Type Mismatch, %s != %s\n", str_token((a)), str_token((b)));\
    } else if (a->type == CHAR && a->value != b-value) {\
        ASSERTION_FAILED("Character Mismatch, %s != %s\n", str_token((a)), str_token((b)));\
    }\
} while(0);

#endif // _TOKEN_TESTS_


#ifdef _LEXER_TESTS_

#include "lexer.h"

/* Compare lexers, and display a message about the failure */
#define assert_equals_lexer(a, b) do {\
    mem_equals((a), (b), Lexer*);\
    if (strcmp((a)->_regex, (b)->_regex)) {\
        ASSERTION_FAILED("Lexers have a Regex Mismatch \"%s\" != \"%s\"n", (a)->_regex, (b)->_regex);\
        ASSERTS_EXIT_CODE = 1;\
    }\
\
    if ((a)->_position, (b)->_position) {\
        ASSERTION_FAILED("Lexers have a Position Mismatch %d != %d\n", (a)->_position, (b)->_position);\
        ASSERTS_EXIT_CODE = 1;\
    }\
} while(0);

#endif // _LEXER_TESTS_

#endif // _REGEX_ASSERTS_H_

typedef int _make_compiler_happy;