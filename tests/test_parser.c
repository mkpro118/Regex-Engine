#define FAIL_FAST

#include "testlib/asserts.h"
#include "testlib/tests.h"

#define REGEX_TOKEN_TESTS
#include "asserts.h"

#include "ast.h"
#include "lexer.h"
#include "parser.h"

// Internal functions from parser.c
Token* peek(Parser* parser);
Token* next(Parser* parser);
int expect(Parser* parser, TokenType type);
ASTNode* parse_base(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_expr(Parser* parser);


// Global vars for tests
char regex[] = "(a|b)*c";
Lexer lexer;
Parser parser;

#define CREATE_PARSER do {\
lexer_init(&lexer, regex);\
parser_init(&parser, &lexer);\
} while (0)

#define DESTROY_PARSER do {\
lexer_free(&lexer);\
parser_free(&parser);\
} while (0)

// Tests below

// Test parser_create
int test_parser_create(void) {
    TEST_BEGIN;

    lexer_init(&lexer, regex);

    Parser* parser = parser_create(&lexer);
    assert_is_not_null(parser);
    assert_is_not_null(parser->tokens);
    assert_equals_int(parser->n_tokens, strlen(regex));
    assert_equals_int(parser->position, 0);

    lexer_free(&lexer);
    parser_free(parser);
    free(parser);
    TEST_END;
}

// Test parser_init
int test_parser_init(void) {
    TEST_BEGIN;
    CREATE_PARSER;

    assert_is_not_null(parser.tokens);
    assert_equals_int(parser.n_tokens, strlen(regex));
    assert_equals_int(parser.position, 0);

    DESTROY_PARSER;
    TEST_END;
}

// Test peek
int test_peek(void) {
    TEST_BEGIN;
    CREATE_PARSER;

    Token* token = peek(&parser);
    assert_is_not_null(token);

    Token expected = {.type=LPAREN};
    assert_equals_token(token, (&expected));

    assert_equals_int(parser.position, 0);

    // Peek again, should have exactly the same token
    Token* token2 = peek(&parser);
    assert_is_not_null(token2);

    assert_equals_ptr(token, token2, Token*);
    assert_equals_int(parser.position, 0);

    // Test with bad input
    token = peek(NULL);
    assert_is_null(token);

    size_t bad_pos = strlen(regex) + 1;
    parser.position = bad_pos;

    // No more tokens to return
    token = peek(&parser);
    assert_is_null(token);
    assert_equals_int(parser.position, bad_pos);

    DESTROY_PARSER;
    TEST_END;
}

// Test next
int test_next(void) {
    TEST_BEGIN;
    CREATE_PARSER;

    Token* token = next(&parser);
    assert_is_not_null(token);

    Token expected = {.type=LPAREN};
    assert_equals_token(token, (&expected));

    assert_equals_int(parser.position, 1);

    // Next again, should have the next token
    token = next(&parser);
    assert_is_not_null(token);

    expected = (Token) {.type=CHAR, .value='a'};

    assert_equals_token(token, (&expected));
    assert_equals_int(parser.position, 2);

    // Test with bad input
    token = next(NULL);
    assert_is_null(token);

    size_t bad_pos = strlen(regex) + 1;
    parser.position = bad_pos;

    // No more tokens to return
    token = next(&parser);
    assert_is_null(token);
    assert_equals_int(parser.position, bad_pos);

    DESTROY_PARSER;
    TEST_END;
}

// Test expect
int test_expect(void) {
    TEST_BEGIN;
    CREATE_PARSER;

    TokenType expected = LPAREN;

    int ret = expect(&parser, expected);
    assert_equals_int(ret, 0);
    assert_equals_int(parser.position, 1);

    // Something that was not expected
    ret = expect(&parser, OR);
    assert_equals_int(ret, -1);
    assert_equals_int(parser.position, 2);

    // Bad input
    ret = expect(NULL, CHAR);
    assert_equals_int(ret, -1);

    // Move the parser's position and try
    parser.position++;
    ret = expect(&parser, CHAR);
    assert_equals_int(ret, 0);

    // Not the expected token
    ret = expect(&parser, LPAREN);
    assert_equals_int(ret, -1);

    DESTROY_PARSER;
    TEST_END;
}

// Test parse_base
// This has two cases
//  - base expands to a CHAR
//  - base expands to a LPAREN expr RPAREN
int test_parse_base(void) {
    TEST_BEGIN;
    // We will use a simpler regex for this test

    // Case 1: base expands to a CHAR
    {
        char regex[] = "abc";
        size_t len = strlen(regex);

        CREATE_PARSER;

        for (char* c = regex; c < &regex[len]; c++) {
            ASTNode* node = parse_base(&parser);
            assert_is_not_null(node);

            assert_equals_int(node->type, CHAR_NODE);
            assert_equals_int(node->extra.character, (*c));

            ast_node_free(node);
        }

        DESTROY_PARSER;
    }

    // Case 2: base expands to a LPAREN expr RPAREN
    // In this case, we are not interested in knowing the value of the
    // parsed expr, rather we only care that the RPAREN was consumed
    // Whether or not expr parses correctly is tested in parse_expr
    {
        char regex[] = "(abc)*";
        // Position of the star in the regex above
        const size_t star_pos = (size_t)(strchr(regex, '*') - regex);

        CREATE_PARSER;

        ASTNode* node = parse_base(&parser);
        assert_is_not_null(node);

        assert_equals_int(parser.position, star_pos);

        ast_node_free(node);
        DESTROY_PARSER;
    }

    // Case 3: Bad Input
    {
        ASTNode* node = parse_base(NULL);
        assert_is_null(node);

        // This regex is not a base non-terminal
        char regex[] = "*";

        CREATE_PARSER;

        node = parse_base(&parser);
        assert_is_null(node);

        ast_node_free(node);
        DESTROY_PARSER;
    }

    TEST_END;
}

// Test parse_factor
// Production rule for factor is
//     non terminal  ::  factor -> base op
//
// This has four cases
//  - op is STAR
//  - op is PLUS
//  - op is QUESTION
//  - op is epsilon
//
// Case 4 is effectively testing whether parse_base works,
// which is tested independently anyway, so we will not do that here
int test_parse_factor(void) {
    TEST_BEGIN;
    // We will use a simpler regex for this test

    // Tests only differ in data, but have similar structure
    // Have the preprocess make them
    #define TEST_CASE(symbol, name) {\
        char regex[] = "a" symbol;\
\
        CREATE_PARSER;\
\
        ASTNode* node = parse_factor(&parser);\
        assert_is_not_null(node);\
\
        assert_equals_int(node->type, name);\
        assert_is_not_null(node->child1);\
        assert_equals_int(node->child1->type, CHAR_NODE);\
        assert_equals_int(node->child1->extra.character, 'a');\
        assert_equals_int(parser.position, 2);\
\
        ast_node_free(node);\
        DESTROY_PARSER;\
    }

    // Case: op is STAR
    TEST_CASE("*", STAR_NODE);

    // Case: op is PLUS
    TEST_CASE("+", PLUS_NODE);

    // Case: op is QUESTION
    TEST_CASE("?", QUESTION_NODE);

    #undef TEST_CASE

    // Case: Bad input
    {
        ASTNode* node = parse_factor(NULL);
        assert_is_null(node);
    }

    TEST_END;
}

// Test parse_term
// Production rule for factor is
//     non terminal  ::  term -> factor factor
//
// This results in two cases
//  - Just the factor itself
//  - Concatenation
//
// Case 1 is effectively testing parse_factor, which is tested independently
// but it will be replicated here because concat does not simply concat
// under some cases, like the OR token, the RPAREN token etc
int test_parse_term(void) {
    TEST_BEGIN;
    // We will use a simpler regex for this test

    // Case: Concatentation of two characters
    {
        char regex[] = "ab";

        CREATE_PARSER;

        ASTNode* node = parse_term(&parser);
        assert_is_not_null(node);

        assert_equals_int(node->type, CONCAT_NODE);
        assert_equals_int(parser.position, 2);

        // Check left child
        assert_equals_int(node->child1->type, CHAR_NODE);
        assert_equals_int(node->child1->extra.character, 'a');

        // Check right child
        assert_equals_int(node->extra.child2->type, CHAR_NODE);
        assert_equals_int(node->extra.child2->extra.character, 'b');

        ast_node_free(node);
        DESTROY_PARSER;
    }

    // Case: Strings that do not have a right child for term
    //       However, it should parse the character 'a'
    #define TEST_CASE(symbol) {\
        char regex[] = "a" symbol;\
\
        CREATE_PARSER;\
\
        ASTNode* node = parse_term(&parser);\
        assert_is_not_null(node);\
\
        assert_equals_int(node->type, CHAR_NODE);\
        assert_equals_int(node->extra.character, 'a');\
        assert_equals_int(parser.position, 1);\
\
        ast_node_free(node);\
        DESTROY_PARSER;\
    }

    // These cases should not be parsed as concatenations
    // Note that LPAREN and CHAR should be parsed at concatenations

    // Case: RPAREN token
    TEST_CASE(")")

    // Case: OR token
    TEST_CASE("|")

    #undef TEST_CASE

    TEST_END;
}

// Test parse_expr
// Production rule for factor is
//     non terminal  ::  expr -> term | term OR term
//
// This results in two cases
//  - expr expands to a single term
//  - expr expands to a alternation of two terms
int test_parse_expr(void) {
    TEST_BEGIN;
    // We will use a simpler regex for this test

    // Case: expr -> term
    {
        char regex[] = "a*";

        CREATE_PARSER;

        ASTNode* node = parse_expr(&parser);
        assert_is_not_null(node);
        assert_equals_int(parser.position, 2);

        assert_equals_int(node->type, STAR_NODE);

        // Check child
        assert_is_not_null(node->child1);
        assert_equals_int(node->child1->type, CHAR_NODE);
        assert_equals_int(node->child1->extra.character, 'a');

        ast_node_free(node);
        DESTROY_PARSER;
    }

    // Case: expr -> term OR term
    {
        // This regex actually checks precedence too,
        // the star should be associated with 'b', not the "a|b"
        char regex[] = "a|b*";

        CREATE_PARSER;

        ASTNode* node = parse_expr(&parser);
        assert_is_not_null(node);
        assert_equals_int(parser.position, 4);

        assert_equals_int(node->type, OR_NODE);

        // Check left child
        assert_is_not_null(node->child1);
        assert_equals_int(node->child1->type, CHAR_NODE);
        assert_equals_int(node->child1->extra.character, 'a');

        // Check right child
        ASTNode* star = node->extra.child2;
        assert_is_not_null(star);
        assert_equals_int(star->type, STAR_NODE);

        // Check star node's child
        ASTNode* star_child = star->child1;
        assert_is_not_null(star_child);
        assert_equals_int(star_child->type, CHAR_NODE);
        assert_equals_int(star_child->extra.character, 'b');

        ast_node_free(node);
        DESTROY_PARSER;
    }

    TEST_END;
}

/* Test parse
 * This is easily one of the most complex tests, and probably should
 * be refactored. At the time of writing, I'd like to be able to have a
 * continous flow, and not jump around with sub functions.
 * I have tried my best to provide a logical flow of the assertions,
 * with inline comments and hopefully self-commenting code.
 */
int test_parse(void) {
    TEST_BEGIN;
    // We will attempt to create all types of AST Nodes here

    char regex[] = "(ab?)|c*|d+";

    /* This will result in an AST that looks like
     *
     * Level 0:                          OR
     *                                   |
     *                          +--------+--------+
     *                          |                 |
     * Level 1:                 OR              PLUS
     *                          |                 |
     *                   +------+-------+         |
     *                   |              |         |
     * Level 2:         CONCAT        STAR     CHAR(d)
     *                   |              |
     *              +----+----+         |
     *              |         |         |
     * Level 3:  CHAR(a)  QUESTION   CHAR(c)
     *                        |
     * Level 4:            CHAR(b)
     */

    // Manually create the AST
    // Nodes created by characters
    ASTNode chars[] = {
        {.type=CHAR_NODE, .extra={.character='a'}},
        {.type=CHAR_NODE, .extra={.character='b'}},
        {.type=CHAR_NODE, .extra={.character='c'}},
        {.type=CHAR_NODE, .extra={.character='d'}},
    };

    // Nodes created by operators
    ASTNode ops[] = {
        {.type = QUESTION_NODE, .child1=&chars[1], .extra={0}},
        {.type = STAR_NODE,     .child1=&chars[2], .extra={0}},
        {.type = PLUS_NODE,     .child1=&chars[3], .extra={0}},
        {.type = CONCAT_NODE,   .child1=NULL,      .extra={0}}, // wired later
        {.type = OR_NODE,       .child1=NULL,      .extra={0}}, // wired later
        {.type = OR_NODE,       .child1=NULL,      .extra={0}}, // wired later
    };

    // Setup CONCAT Node
    ops[3].child1 = &chars[0];      // Character('a') node
    ops[3].extra.child2 = &ops[0];  // Question("b?") node

    // Setup the first or leftmost OR Node (for "(ab?)|c*")
    ops[4].child1 = &ops[3];        // Concat("ab?") node
    ops[4].extra.child2 = &ops[1];  // Star("c*") node

    // Setup the second or rightmost OR Node
    ops[5].child1 = &ops[4];        // Or("(ab?)|c*") node
    ops[5].extra.child2 = &ops[2];  // Plus("d+") node

    // Start test
    CREATE_PARSER;

    // Testing using a DFS traversal on the resulting AST

    ASTNode* root = parse(&parser);

    // Level 0: Check the root
    assert_is_not_null(root);
    assert_equals_int(root->type, ops[5].type); // Root should be OR node

    // Root node's children should not be null
    assert_is_not_null(root->child1);
    assert_is_not_null(root->extra.child2);

    // Level 1: Check root node's left child, i.e OR node
    ASTNode* or = root->child1;
    assert_equals_int(or->type, ops[4].type);

    // OR's children should not be null
    assert_is_not_null(or->child1);
    assert_is_not_null(or->extra.child2);

    // Level 2: Check OR's left child, i.e. Concat node
    ASTNode* concat = or->child1;
    assert_equals_int(concat->type, ops[3].type);

    // Concat's children should not be null
    assert_is_not_null(concat->child1);
    assert_is_not_null(concat->extra.child2);

    // Level 3: Check concat's left child, i.e. Char(a) node
    ASTNode* char_a = concat->child1;
    assert_equals_int(char_a->type, chars[0].type);
    assert_equals_int(char_a->extra.character, chars[0].extra.character);

    // Level 3: Check concat's right child, i.e. Question node
    ASTNode* question = concat->extra.child2;
    assert_equals_int(question->type, ops[0].type);

    // Question's child should not be null
    assert_is_not_null(question->child1);

    // Level 4: Check question's child, i.e Char(b) node
    ASTNode* char_b = question->child1;
    assert_equals_int(char_b->type, chars[1].type);
    assert_equals_int(char_b->extra.character, chars[1].extra.character);

    // (Go back up the tree to Level 1 ...)

    // Level 2: Check OR's right child, i.e. Star("c*") node
    ASTNode* star = or->extra.child2;
    assert_equals_int(star->type, ops[1].type);

    // Star's child should not be null
    assert_is_not_null(star->child1);

    // Level 3: Check star's child, i.e. Char(c) node
    ASTNode* char_c = star->child1;
    assert_equals_int(char_c->type, chars[2].type);
    assert_equals_int(char_c->extra.character, chars[2].extra.character);

    // (Go back up the tree to Level 0 ...)

    // Level 1: Check the root node's right child, i.e. Plus("d+") node
    ASTNode* plus = root->extra.child2;
    assert_equals_int(plus->type, ops[2].type);

    // Plus's child should not be null
    assert_is_not_null(plus->child1);

    // Level 2: Check Plus's child, i.e. Char(d) node
    ASTNode* char_d = plus->child1;
    assert_equals_int(char_d->type, chars[3].type);
    assert_equals_int(char_d->extra.character, chars[3].extra.character);

    // Done, all nodes of the AST have been tested.

    ast_node_free(root); // This should recursively free all the nodes
    DESTROY_PARSER;
    TEST_END;
}

Test tests[] = {
    {.name="test_parser_create", .func=test_parser_create},
    {.name="test_parser_init", .func=test_parser_init},
    {.name="test_peek", .func=test_peek},
    {.name="test_next", .func=test_next},
    {.name="test_expect", .func=test_expect},
    {.name="test_parse_base", .func=test_parse_base},
    {.name="test_parse_factor", .func=test_parse_factor},
    {.name="test_parse_term", .func=test_parse_term},
    {.name="test_parse_expr", .func=test_parse_expr},
    {.name="test_parse", .func=test_parse},
    {.name = NULL},
};


int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
