#ifndef REGEX_TOKEN_H
#define REGEX_TOKEN_H

typedef enum TokenType {
    CHAR,
    LPAREN,
    OR,
    PLUS,
    QUESTION,
    RPAREN,
    STAR,
    EOF_,
    ERROR,
} TokenType;

typedef struct Token {
    TokenType type;
    char value;
} Token;

const char* str_token(Token* token);

#endif // REGEX_TOKEN_H
