#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

/**
 * Returns the next token with advancing the parser's position
 *
 * @param  parser The parser to peek with
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* peek(Parser* parser) {
    // TODO
}


/**
 * Returns the next token and advances the parser's position
 *
 * @param parser The parser to get the next token from
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* next(Parser* parser) {
    // TODO
}


/**
 * Ensure that the next token is of a given type
 *
 * @param  parser The parser to check with
 *
 * @return 0 if the next token is of the required type, -1 otherwise
 */
int expect(Parser* parser, ASTNodeType type) {
    // TODO
}


/**
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
ASTNode* parse_base(Parser* parser) {
    // TODO
}


/**
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
ASTNode* parse_factor(Parser* parser) {
    // TODO
}


/**
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
ASTNode* parse_term(Parser* parser) {
    // TODO
}


/**
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
ASTNode* parse_expr(Parser* parser) {
    // TODO
}
