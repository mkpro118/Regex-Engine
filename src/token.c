#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "token.h"

const char* str_token(Token* token) {
    static const char char_token_template[] = "CHAR('.')";
    static char buf[sizeof(char_token_template)];

    static const char* tokenStr[] = {
        char_token_template,
        "LPAREN",
        "OR",
        "PLUS",
        "QUESTION",
        "RPAREN",
        "STAR"
    };

    if (token->type != CHAR) {
        return tokenStr[token->type];
    }

    strncpy(buf, char_token_template, sizeof(char_token_template));
    buf[6] = token->value;

    return buf;
}
