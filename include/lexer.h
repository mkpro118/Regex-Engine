#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdlib.h>
#include <sys/types.h>
#include <token.h>

typedef struct Lexer {
    const char* _regex;
    size_t _position;
    ssize_t _regex_lex;
} Lexer;

Lexer* lexer_create(char* regex);
int lexer_init(Lexer* lexer, char* regex);
int next_token(Lexer* lexer, Token* token);
int tokenize(Lexer* lexer, Token* buf, size_t buf_size);
Token* tokenize_all(Lexer* lexer, size_t* size);

#endif
