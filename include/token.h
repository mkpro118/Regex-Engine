#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum TokenType {
    CHAR,
    LPAREN,
    OR,
    PLUS,
    QUESTION,
    RPAREN,
    STAR,
    EOF,
    ERROR,
} TokenType;

typedef struct Token {
    TokenType type;
    char value;
} Token;

const char* str_token(Token* token);

#endif // _TOKEN_H_
