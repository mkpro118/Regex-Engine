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
