#ifndef REGEX_MAIN_H
#define REGEX_MAIN_H

#include <stdbool.h>

#include "ast.h"
#include "converter.h"
#include "lexer.h"
#include "nfa.h"
#include "nfa_state.h"
#include "parser.h"
#include "token.h"

/**
 * Represents a Regex pattern
 *
 * Members
 *     - nfa: The internal Non-deterministic finite automata.
 *            This field is NULL until the regex is compiled.
 *     - is_compiled: Whether or not the regex has been compiled.
 *     - pattern: The regex pattern that was compiled to create the `nfa`.
 */
typedef struct Regex {
    NFA* nfa;
    bool is_compiled;
    char* pattern;
} Regex;

/**
 * Create a heap allocated and initialized regex buffer.
 *
 * @param  pattern The pattern to initialize regex buffer with
 *                 can be NULL if compilation is not desired.
 *
 * @return A pointer to the heap allocated regex buffer
 */
Regex* regex_create(char* pattern);

/**
 * Initialize a given regex buffer.
 *
 * @param  regex_buf The buffer to initialize
 * @param  pattern   The pattern to initialize the regex buffer with
 *                   can be NULL if compilation is not desired.
 *
 * @return 0 on success, -1 on failure
 */
int regex_init(Regex* regex_buf, char* pattern);

/**
 * Compile a given regex pattern.
 *
 * @param  regex_buf   A pointer to the a regex buffer
 * @param  pattern The regex pattern
 *
 * @return 0 on success, -1 on failure
 */
int regex_compile(Regex* regex_buf, char* pattern);

/**
 * Test whether the given string matches the given regex.
 *
 * @param  regex_buf  The regex buffer to match with
 * @param  string     The string to match
 *
 * @return true if the string matches the pattern specified by the regex_buf,
 *         false if it doesn't or if the input is invalid
 */
bool regex_match(Regex* regex_buf, char* string);

/**
 * Release the memory used by the given regex structure
 *
 * @param  regex_buf
 */
void regex_free(Regex* regex_buf);

#endif // REGEX_MAIN_H
