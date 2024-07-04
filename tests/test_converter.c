#include <stdbool.h>

#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

ASTNode* root;
NFA* nfa;

int setup() {
    root = NULL;
    nfa = NULL;
    return 0;
}

int teardown() {
    if (root != NULL) {
        ast_node_free(root);
    }
    if (nfa != NULL) {
        nfa_free(nfa);
        free(nfa);
        nfa = NULL;
    }
    return 0;
}

int test_convert_char_node() {
    TEST_BEGIN;

    root = ast_node_create(CHAR_NODE);
    root->extra.character = 'a';

    nfa = convert_ast_to_nfa(root);

    ast_node_free(root);
    root = NULL;

    assert_is_not_null(nfa);
    assert_is_not_null(nfa->start_state);
    assert_is_not_null(nfa->final_states);
    assert_equals_int(nfa->final_states->size, 1);
    
    bool match = nfa_match(nfa, "a");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_star_node() {
    TEST_BEGIN;

    root = ast_node_create(STAR_NODE);
    root->child1 = ast_node_create(CHAR_NODE);
    root->child1->extra.character = 'a';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "a");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aa");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aaa");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_plus_node() {
    TEST_BEGIN;

    root = ast_node_create(PLUS_NODE);
    root->child1 = ast_node_create(CHAR_NODE);
    root->child1->extra.character = 'a';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "a");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aa");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aaa");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_question_node() {
    TEST_BEGIN;

    root = ast_node_create(QUESTION_NODE);
    root->child1 = ast_node_create(CHAR_NODE);
    root->child1->extra.character = 'a';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "a");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aa");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_or_node() {
    TEST_BEGIN;

    root = ast_node_create(OR_NODE);
    root->child1 = ast_node_create(CHAR_NODE);
    root->child1->extra.character = 'a';
    root->extra.child2 = ast_node_create(CHAR_NODE);
    root->extra.child2->extra.character = 'b';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "a");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "ab");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "c");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_concat_node() {
    TEST_BEGIN;

    root = ast_node_create(CONCAT_NODE);
    root->child1 = ast_node_create(CHAR_NODE);
    root->child1->extra.character = 'a';
    root->extra.child2 = ast_node_create(CHAR_NODE);
    root->extra.child2->extra.character = 'b';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "ab");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "a");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "abc");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

int test_convert_complex_ast() {
    TEST_BEGIN;

    // Creating AST for (a|b)*c
    root = ast_node_create(CONCAT_NODE);
    root->child1 = ast_node_create(STAR_NODE);
    root->child1->child1 = ast_node_create(OR_NODE);
    root->child1->child1->child1 = ast_node_create(CHAR_NODE);
    root->child1->child1->child1->extra.character = 'a';
    root->child1->child1->extra.child2 = ast_node_create(CHAR_NODE);
    root->child1->child1->extra.child2->extra.character = 'b';
    root->extra.child2 = ast_node_create(CHAR_NODE);
    root->extra.child2->extra.character = 'c';

    nfa = convert_ast_to_nfa(root);

    assert_is_not_null(nfa);
    
    bool match = nfa_match(nfa, "c");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "ac");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "bc");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "aabbabaabc");
    assert_equals_int(match, true);

    match = nfa_match(nfa, "");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "a");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "ab");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "abcd");
    assert_equals_int(match, false);

    ast_node_free(root);
    root = NULL;

    nfa_free(nfa);
    free(nfa);
    nfa = NULL;

    TEST_END;
}

Test tests[] = {
    {.name="test_convert_char_node", .func=test_convert_char_node},
    {.name="test_convert_star_node", .func=test_convert_star_node},
    {.name="test_convert_plus_node", .func=test_convert_plus_node},
    {.name="test_convert_question_node", .func=test_convert_question_node},
    {.name="test_convert_or_node", .func=test_convert_or_node},
    {.name="test_convert_concat_node", .func=test_convert_concat_node},
    {.name="test_convert_complex_ast", .func=test_convert_complex_ast},
    {.name=NULL, .func=NULL}
};

int main(int argc, char* argv[]) {
    setup();
    int result = default_main(&argv[1], argc - 1);
    teardown();
    return result;
}
