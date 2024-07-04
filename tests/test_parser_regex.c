#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

#define ALLOCATE_PARSER {\
lexer_init(&lexer, regex);\
parser_init(&parser, &lexer);\
}\
ASTNode* result = parse(&parser)

#define DEALLOCATE_PARSER do {\
ast_node_free(result);\
parser_free(&parser);\
lexer_free(&lexer);\
} while(0)

int compare_ast(ASTNode* node1, ASTNode* node2) {
    if (node1 == NULL && node2 == NULL) return 1;
    if (node1 == NULL || node2 == NULL) return 0;
    if (node1->type != node2->type) return 0;

    switch (node1->type) {
        case CHAR_NODE:
            return node1->extra.character == node2->extra.character;
        case OR_NODE:
        case CONCAT_NODE:
            return compare_ast(node1->child1, node2->child1) &&
                   compare_ast(node1->extra.child2, node2->extra.child2);
        default:
            return compare_ast(node1->child1, node2->child1);
    }
}

Lexer lexer;
Parser parser;

// Test cases

int test_simple_char() {
    TEST_BEGIN;

    char* regex = "a";
    ALLOCATE_PARSER;

    ASTNode expected = {.type = CHAR_NODE, .extra = {.character = 'a'}};

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_simple_concatenation() {
    TEST_BEGIN;

    char* regex = "ab";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = CONCAT_NODE,
        .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
        .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'b'}}
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_simple_alternation() {
    TEST_BEGIN;

    char* regex = "a|b";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = OR_NODE,
        .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
        .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'b'}}
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_complex_expression() {
    TEST_BEGIN;

    char* regex = "(a|b)*c+d?";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = CONCAT_NODE,
        .child1 = &(ASTNode){
            .type = CONCAT_NODE,
            .child1 = &(ASTNode){
                .type = STAR_NODE,
                .child1 = &(ASTNode){
                    .type = OR_NODE,
                    .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
                    .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'b'}}
                },
            .extra = {0},
            },
            .extra.child2 = &(ASTNode){
                .type = PLUS_NODE,
                .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'c'}},
                .extra = {0}
            }
        },
        .extra.child2 = &(ASTNode){
            .type = QUESTION_NODE,
            .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'd'}},
            .extra = {0}
        }
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_nested_parentheses() {
    TEST_BEGIN;

    char* regex = "((a|b)c|(d|e)f)g";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = CONCAT_NODE,
        .child1 = &(ASTNode){
            .type = OR_NODE,
            .child1 = &(ASTNode){
                .type = CONCAT_NODE,
                .child1 = &(ASTNode){
                    .type = OR_NODE,
                    .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
                    .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'b'}}
                },
                .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'c'}}
            },
            .extra.child2 = &(ASTNode){
                .type = CONCAT_NODE,
                .child1 = &(ASTNode){
                    .type = OR_NODE,
                    .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'd'}},
                    .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'e'}}
                },
                .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'f'}}
            }
        },
        .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'g'}}
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_multiple_alternations() {
    TEST_BEGIN;

    char* regex = "a|b|c|d|e";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = OR_NODE,
        .child1 = &(ASTNode){
            .type = OR_NODE,
            .child1 = &(ASTNode){
                .type = OR_NODE,
                .child1 = &(ASTNode){
                    .type = OR_NODE,
                    .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
                    .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'b'}}
                },
                .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'c'}}
            },
            .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'd'}}
        },
        .extra.child2 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'e'}}
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_empty_parentheses() {
    TEST_BEGIN;

    char* regex = "a()b";
    ALLOCATE_PARSER;

    assert_is_null(result);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_unmatched_parentheses() {
    TEST_BEGIN;

    char* regex = "a(b";
    ALLOCATE_PARSER;

    assert_is_null(result);

    DEALLOCATE_PARSER;
    TEST_END;
}

int test_consecutive_operators() {
    TEST_BEGIN;

    char* regex = "a**+?";
    ALLOCATE_PARSER;

    ASTNode expected = {
        .type = QUESTION_NODE,
        .child1 = &(ASTNode){
            .type = PLUS_NODE,
            .child1 = &(ASTNode){
                .type = STAR_NODE,
                .child1 = &(ASTNode){
                    .type = STAR_NODE,
                    .child1 = &(ASTNode){.type = CHAR_NODE, .extra = {.character = 'a'}},
                    .extra={0}
                },
                .extra={0}
            },
            .extra={0}
        },
        .extra={0}
    };

    int success = compare_ast(result, &expected);
    assert_equals_int(success, 1);

    DEALLOCATE_PARSER;
    TEST_END;
}

Test tests[] = {
    {.name="test_simple_char", .func=test_simple_char},
    {.name="test_simple_concatenation", .func=test_simple_concatenation},
    {.name="test_simple_alternation", .func=test_simple_alternation},
    {.name="test_complex_expression", .func=test_complex_expression},
    {.name="test_nested_parentheses", .func=test_nested_parentheses},
    {.name="test_multiple_alternations", .func=test_multiple_alternations},
    {.name="test_empty_parentheses", .func=test_empty_parentheses},
    {.name="test_unmatched_parentheses", .func=test_unmatched_parentheses},
    {.name="test_consecutive_operators", .func=test_consecutive_operators},
    {.name=NULL},
    {.name=NULL},
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
