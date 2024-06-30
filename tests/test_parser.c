#define FAIL_FAST

#include "testlib/asserts.h"
#include "testlib/tests.h"

#define REGEX_TOKEN_TESTS
#include "asserts.h"

#include "lexer.h"
#include "parser.h"

// Internal functions from parser.c
Token* peek(Parser* parser);
Token* next(Parser* parser);
int expect(Parser* parser, ASTNodeType type);
ASTNode* parse_base(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_expr(Parser* parser);


// Global vars for tests
char regex[] = "(a|b)*c";
Lexer lexer;
Parser parser;


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
    TEST_END;
}

// Test parser_init
int test_parser_init(void) {
    TEST_BEGIN;

    lexer_init(&lexer, regex);
    parser_init(&parser, &lexer);

    assert_is_not_null(parser.tokens);
    assert_equals_int(parser.n_tokens, strlen(regex));
    assert_equals_int(parser.position, 0);

    lexer_free(&lexer);
    parser_free(&parser);
    TEST_END;
}

// Test peek
int test_peek(void) {
    TEST_BEGIN;

    lexer_init(&lexer, regex);
    parser_init(&parser, &lexer);

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

    lexer_free(&lexer);
    parser_free(&parser);
    TEST_END;
}

// Test next
int test_next(void) {
    TEST_BEGIN;
    lexer_init(&lexer, regex);
    parser_init(&parser, &lexer);

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

    lexer_free(&lexer);
    parser_free(&parser);
    TEST_END;
}

// Test expect
int test_expect(void) {
    TEST_BEGIN;
    TEST_END;
}

// Test parse_base
int test_parse_base(void) {
    TEST_BEGIN;
    TEST_END;
}

// Test parse_factor
int test_parse_factor(void) {
    TEST_BEGIN;
    TEST_END;
}

// Test parse_term
int test_parse_term(void) {
    TEST_BEGIN;
    TEST_END;
}

// Test parse_expr
int test_parse_expr(void) {
    TEST_BEGIN;
    TEST_END;
}

// Test parse
int test_parse(void) {
    TEST_BEGIN;
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
