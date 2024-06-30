#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

/**
 * TODO: WIP
 * Returns the next token with advancing the parser's position
 *
 * @param  parser The parser to peek with
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* peek(Parser* parser){
    return parser != NULL ? NULL : NULL;
}


/**
 * TODO: WIP
 * Returns the next token and advances the parser's position
 *
 * @param parser The parser to get the next token from
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* next(Parser* parser){
    return parser != NULL ? NULL : NULL;
}


/**
 * TODO: WIP
 * Ensure that the next token is of a given type
 *
 * @param  parser The parser to check with
 *
 * @return 0 if the next token is of the required type, -1 otherwise
 */
int expect(Parser* parser, TokenType type){
    return parser != NULL && type ? 0 : 0;
}


/**
 * TODO: WIP
 * Parse the `base` non-terminal.
 *
 * See the regex CFG on
 * https://github.com/mkpro118/Regex-Engine/issues/6#issue-2337160940
 *
 * The relevant production is reproduced below
 *     non terminal  ::  base ->   CHAR  |  LPAREN expr RPAREN
 *
 * @param  parser The parser to operate on
 *
 * @return The root of the AST created by parsing the `base` non-terminal
 */
ASTNode* parse_base(Parser* parser){
    return parser != NULL ? NULL : NULL;
}


/**
 * TODO: WIP
 * Parse the `factor` non-terminal.
 *
 * See the regex CFG on
 * https://github.com/mkpro118/Regex-Engine/issues/6#issue-2337160940
 *
 * The relevant production is reproduced below
 *     non terminal  ::  factor ->   base op  |   factor op
 *
 * @param  parser The parser to operate on
 *
 * @return The root of the AST created by parsing the `factor` non-terminal
 */
ASTNode* parse_factor(Parser* parser){
    return parser != NULL ? NULL : NULL;
}


/**
 * TODO: WIP
 * Parse the `term` non-terminal.
 *
 * See the regex CFG on
 * https://github.com/mkpro118/Regex-Engine/issues/6#issue-2337160940
 *
 * The relevant production is reproduced below
 *     non terminal  ::  term -> factor factor
 *
 * @param  parser The parser to operate on
 *
 * @return The root of the AST created by parsing the `term` non-terminal
 */
ASTNode* parse_term(Parser* parser){
    return parser != NULL ? NULL : NULL;
}


/**
 * TODO: WIP
 * Parse the `expr` non-terminal.
 *
 * See the regex CFG on
 * https://github.com/mkpro118/Regex-Engine/issues/6#issue-2337160940
 *
 * The relevant production is reproduced below
 *     non terminal  ::  expr -> term OR term
 *
 * @param  parser The parser to operate on
 *
 * @return The root of the AST created by parsing the `expr` non-terminal
 */
ASTNode* parse_expr(Parser* parser){
    return parser != NULL ? NULL : NULL;
}

// Create a heap allocated parser from the given Lexer
Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));

    if (parser_init(parser, lexer) < 0) {
        free(parser);
        return NULL;
    }

    return parser;
}

// Initialize the given parse using the given Lexer
int parser_init(Parser* parser, Lexer* lexer) {
    if (parser == NULL) {
        return -1;
    }

    parser->tokens = tokenize_all(lexer, &parser->n_tokens);

    if (parser->tokens == NULL || parser->n_tokens <= 0) {
        return -1;
    }

    return parser->position = 0;
}

// Release the memory used by this parser.
void parser_free(Parser* parser) {
    if (parser)
        return;
    return;
}

// Create a AST by parsing the tokens in the given parser
ASTNode* parse(Parser* parser) {
    return parser != NULL ? NULL : NULL;
}
