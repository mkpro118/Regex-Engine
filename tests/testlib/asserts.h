#ifndef _REGEX_ASSERTS_H_
#define _REGEX_ASSERTS_H_

#include "token.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A zero exit code refers to success on all assertions.
   Any other value refers to a failure on at least one assertion. */
extern int ASSERTS_EXIT_CODE;

#ifdef FAIL_FAST
#define ASSERTION_FAILED(fmt, a, b) do {\
    fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
    fprintf(stderr, fmt, (a), (b));\
    ASSERTS_EXIT_CODE = 1;\
    return;\
} while(0);
#else
#define ASSERTION_FAILED(fmt, a, b) do {\
    fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
    fprintf(stderr, fmt, (a), (b));\
    ASSERTS_EXIT_CODE = 1;\
} while(0);
#endif // FAIL_FAST

/* If they refer to the same memory then they are the same thing */
#define mem_equals(a, b, type) if ((type) (a) == ((type) (b))) break;

/* Compare integers, and display a message about the failure */
#define assert_equals_int(a, b) do {\
    if ((a) != (b)) {\
        ASSERTION_FAILED("%d != %d\n", (a), (b));\
    }\
} while(0);

/* Compare strings, and display a message about the failure */
#define assert_equals_str(a, b) do {\
    mem_equals((a), (b), char*);\
    if (strcmp((char*) (a), (char*) (b))) {\
        ASSERTION_FAILED("\"%s\" != \"%s\"\n", (a), (b));\
    }\
} while(0);

/* Compare tokens, and display a message about the failure */
#define assert_equals_token(a, b) do {\
    mem_equals((a), (b), Token*);\
    if (a->type != b->type) {\
        ASSERTION_FAILED("Token Type Mismatch, %s != %s\n", str_token((a)), str_token((b)));\
    } else if (a->type == CHAR && a->value != b-value) {\
        ASSERTION_FAILED("Character Mismatch, %s != %s\n", str_token((a)), str_token((b)));\
    }\
} while(0);

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

// #undef mem_equals
// #undef ASSERTION_FAILED
#endif // _REGEX_ASSERTS_H_
