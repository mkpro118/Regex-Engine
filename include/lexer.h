#ifndef REGEX_LEXER_H
#define REGEX_LEXER_H

#include <stdlib.h>
#include <sys/types.h>
#include <token.h>

/**
 * State is maintained by Lexers
 *
 * Members
 *     _regex: The regex string to be scanned
 *     _position: The current position (state) of the lexer
 */
typedef struct Lexer {
    const char* _regex;
    size_t _position;
    size_t _regex_len;
} Lexer;


/**
 * Creates and Initializes a lexer given a regex string
 *
 * @param  regex The regex string to scan
 *
 * @return A pointer to a heap allocated lexer
 */
Lexer* lexer_create(char* regex);

/**
 * Releases memory used by the given lexer.
 *
 * @param  lexer The lexer to deallocate
 */
void lexer_free(Lexer* lexer);

/**
 * Initializes a lexer given a regex string
 *
 * @param  regex The regex string to scan
 *
 * @return  0 on success
 *         -1 on failure
 */
int lexer_init(Lexer* lexer, char* regex);

/**
 * Finds the next token with the given lexer state
 *
 * @param  lexer The lexer to scan tokens with
 * @param  token The token buffer to fill
 *
 * @return   0 on success
 *          -1 on failure
 */
int next_token(Lexer* lexer, Token* token);


/**
 * Fills the given buffer with at most buf_size Tokens
 *
 * @param  lexer    The lexer to scan tokens with
 * @param  buf      The token array to fill
 * @param  buf_size The size of the token array
 *
 * @return  number of tokens scanned on success (may be 0)
 *          -1 on failure
 */
int tokenize(Lexer* lexer, Token* buf, size_t buf_size);


/**
 * Returns dynamically allocated array of all the tokens from the given lexer
 *
 * @param  lexer The lexer to scan tokens with
 * @param  size  The size of the array returned
 *
 * @return A pointer an array of scanned tokens on success
 *         NULL on failure
 */
Token* tokenize_all(Lexer* lexer, size_t* size);

#endif // REGEX_LEXER_H
