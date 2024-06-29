#include <stdlib.h>
#include <string.h>
#include <portability.h>
#include <ctype.h>

#include "lexer.h"

// Create a Lexer with the given regex
Lexer* lexer_create(char* regex) {
    Lexer* lexer = malloc(sizeof(Lexer));

    // Check if malloc failed
    if (lexer == NULL) {
        return NULL;
    }

    lexer_init(lexer, regex);
    return lexer;
}

// Initialize a Lexer with the given regex
int lexer_init(Lexer* lexer, char* regex) {
    char* cpy = strdup(regex);

    if (cpy == NULL) {
        return -1;
    }

    lexer->_regex = cpy;
    lexer->_position = 0;
    lexer->_regex_lex = strlen(cpy);

    return 0;
}

// Finds the next token with the given lexer state
// Result is filled in the token parameter
int next_token(Lexer* lexer, Token* token) {
    token->value = 0; // Reset token value regardless

    if (lexer->_position >= lexer->_regex_lex) {
        token->type = EOF;
        return 0;
    }

    char character = lexer->_regex[lexer->_position++];

    switch (character) {
    case '(':
        token->type = LPAREN;
        break;

    case '|':
        token->type = OR;
        break;

    case '+':
        token->type = PLUS;
        break;

    case '?':
        token->type = QUESTION;
        break;

    case ')':
        token->type = RPAREN;
        break;

    case '*':
        token->type = STAR;
        break;

    default:
        // We do not support non-printable characters
        if (!isprint(character)) {
            token->type = ERROR;
            return -1;
        }

        token->type = CHAR;
        token->value = character;
    }

    return 0;
}

// Fills the given buffer with at most buf_size Tokens
int tokenize(Lexer* lexer, Token* buf, size_t buf_size) {
    size_t n_tokens = 0; // Number of tokens filled

    for (; n_tokens < buf_size; n_tokens++) {
        // if next_token fails, then tokenize fails
        if (next_token(lexer, &buf[n_tokens]) < 0) {
            return -1;
        }

        // Stop scanning after EOF, exiting early
        if (buf[n_tokens].type == EOF_) {
            break;
        }
    }

    return n_tokens;
}

// Returns dynamically allocated array of all the tokens from the given lexer
Token* tokenize_all(Lexer* lexer, size_t* size) {
    int remaining_chars = lexer->_regex_lex - lexer->_position;

    if (remaining_chars <= 0) {
        *size = 0;
        return NULL;
    }

    // The returned array size is equal to the number of remaining characters
    *size = remaining_chars;

    Token* tokens = malloc(sizeof(Token) * remaining_chars);

    for (int i = 0; i < remaining_chars; i++) {
        // If next_token fails, then tokenize_all fails
        if (next_token(lexer, &tokens[i]) < 0) {
            // Free tokens array to not leak memory
            free(tokens);

            // Reset array size due to failure
            *size = 0;
            return NULL;
        }
    }

    return tokens;
}
