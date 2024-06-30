#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "ast.h"

// Test creating AST dynamically
int test_ast_create() {
    TEST_BEGIN;
    ASTNodeType types[] = {
        CHAR_NODE, STAR_NODE, PLUS_NODE,
        OR_NODE, QUESTION_NODE, CONCAT_NODE,
    };

    size_t n_types = sizeof(types) / sizeof(ASTNodeType);

    for (size_t i = 0; i < n_types; i++) {
        ASTNode* node = ast_node_create(types[i]);

        assert_is_not_null(node);
        assert_equals_int(node->type, types[i]);
        ast_node_free(node);
    }
    TEST_END;
}

int test_ast_init() {
    TEST_BEGIN;
    ASTNodeType types[] = {
        CHAR_NODE, STAR_NODE, PLUS_NODE,
        OR_NODE, QUESTION_NODE, CONCAT_NODE,
    };

    size_t n_types = sizeof(types) / sizeof(ASTNodeType);

    ASTNode node;
    for (size_t i = 0; i < n_types; i++) {
        int ret = ast_node_init(&node, types[i]);

        assert_equals_int(ret, 0);
        assert_equals_int(node.type, types[i]);
    }

    int ret = ast_node_init(NULL, *types);
    assert_equals_int(ret, -1);

    TEST_END;
}


Test tests[] = {
    {.name="test_ast_create", .func=test_ast_create},
    {.name="test_ast_init", .func=test_ast_init},
    {.name=NULL},
};


int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
