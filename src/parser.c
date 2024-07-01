#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

// Forward declaration of parsing functions
ASTNode* parse_base(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_expr(Parser* parser);

/**
 * Returns the next token with advancing the parser's position
 *
 * @param  parser The parser to peek with
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* peek(Parser* parser){
    if (parser == NULL) {
        return NULL;
    }

    if (parser->position >= parser->n_tokens) {
        return NULL;
    }

    return &(parser->tokens[parser->position]);
}


/**
 * Returns the next token and advances the parser's position
 *
 * @param parser The parser to get the next token from
 *
 * @return A pointer to the next token if it exists, NULL otherwise
 */
Token* next(Parser* parser){
    if (parser == NULL) {
        return NULL;
    }

    if (parser->position >= parser->n_tokens) {
        return NULL;
    }

    return &(parser->tokens[parser->position++]);
}


/**
 * Ensure that the next token is of a given type
 *
 * @param  parser The parser to check with
 *
 * @return 0 if the next token is of the required type, -1 otherwise
 */
int expect(Parser* parser, TokenType type){
    if (parser == NULL) {
        return -1;
    }

    if (parser->position >= parser->n_tokens) {
        return -1;
    }

    Token* token = next(parser);
    if (token == NULL) {
        return type == EOF_ ? 0 : -1;
    }

    return token->type == type ? 0 : -1;
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
    if (parser == NULL) {
        return NULL;
    }

    Token* token = next(parser);
    ASTNode* node = NULL;

    switch (token->type) {
    case CHAR:
        node = ast_node_create(CHAR_NODE);
        node->extra.character = token->value;
        break;

    case LPAREN:;
        Token* next = peek(parser);
        if (next != NULL && next->type == RPAREN) {
            return NULL;
        }

        node = parse_expr(parser);

        if (expect(parser, RPAREN) < 0) {
            ast_node_free(node);
            return NULL;
        }

        break;

    default:
        // Error
        return NULL;
    }

    return node;
}


/**
 * Parse the `factor` non-terminal.
 *
 * See the regex CFG on
 * https://github.com/mkpro118/Regex-Engine/issues/6#issue-2337160940
 *
 * The relevant production is reproduced below
 *     non terminal  ::  factor ->   base op | epsilon
 *
 * @param  parser The parser to operate on
 *
 * @return The root of the AST created by parsing the `factor` non-terminal
 */
ASTNode* parse_factor(Parser* parser) {
    if (parser == NULL) {
        return NULL;
    }

    ASTNode* node = parse_base(parser);

    Token* token;

    while ((token = peek(parser))) {
        ASTNode* parent = NULL;

        switch (token->type) {
        case STAR:
            parent = ast_node_create(STAR_NODE);
            break;

        case PLUS:
            parent = ast_node_create(PLUS_NODE);
            break;

        case QUESTION:
            parent = ast_node_create(QUESTION_NODE);
            break;

        default:
            return node;
        }

        parent->child1 = node;
        node = parent;
        next(parser);
    }

    return node;
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
    if (parser == NULL) {
        return NULL;
    }

    ASTNode* left = parse_factor(parser);

    Token* token;

    while ((token = peek(parser))) {
        if (token->type == OR || token->type == RPAREN) {
            return left;
        }

        ASTNode* right = parse_factor(parser);
        if (right == NULL) {
            ast_node_free(left);
            return NULL;
        }

        ASTNode* concat = ast_node_create(CONCAT_NODE);
        if (concat == NULL) {
            ast_node_free(left);
            ast_node_free(right);
            return NULL;
        }


        concat->child1 = left;
        concat->extra.child2 = right;

        left = concat;
    }

    return left;
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
ASTNode* parse_expr(Parser* parser){
    if (parser == NULL) {
        return NULL;
    }


    ASTNode* left = parse_term(parser);

    Token* token;

    while ((token = peek(parser)) && token->type == OR) {
        // This is to consume the OR token
        next(parser);

        ASTNode* right = parse_term(parser);
        if (right == NULL) {
            ast_node_free(left);
            return NULL;
        }

        ASTNode* or = ast_node_create(OR_NODE);
        if (or == NULL) {
            ast_node_free(left);
            ast_node_free(right);
            return NULL;
        }


        or->child1 = left;
        or->extra.child2 = right;

        left = or;
    }

    return left;
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
    if (parser == NULL) {
        return;
    }

    free(parser->tokens);
}

// Create a AST by parsing the tokens in the given parser
ASTNode* parse(Parser* parser) {
    return parse_expr(parser);
}
