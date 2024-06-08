#include <stdlib.h>
#include <string.h>
#include <portability.h>

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

    return 0;
}
