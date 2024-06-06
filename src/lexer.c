#include <stdlib.h>

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
