#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"

typedef struct Parser {
    Token* tokens;
    size_t n_tokens;

    size_t position;
} Parser;


/**
 * Create a heap allocated parser from the given Lexer
 *
 * @param  lexer A Pointer to the Lexer to use for obtaining tokens
 * @return A pointer to a heap allocated Parser on success,
 *         NULL on failure
 */
Parser* parser_create(Lexer* lexer);

/**
 * Initialize the given parse using the given Lexer
 *
 * @param parser A Pointer to the parser to initialize
 * @param lexer  A Pointer to the Lexer to use for obtaining tokens
 *
 * @return 0 on success, -1 on failure
 */
int parser_init(Parser* parser, Lexer* lexer);

/**
 * Create a AST by parsing the tokens in the given parser
 *
 * @param  parser A pointer to the parser to create the AST from
 * 
 * @return A pointer to a heap allocated AST root on success,
 *         NULL on failure
 */
ASTNode* parse(Parser* parser);

/**
 * Release the memory used by this parser.
 *
 * @param parser The parser to deallocate
 */
void parser_free(Parser* parser);

#endif // REGEX_PARSER_H
