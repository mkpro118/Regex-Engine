#include <stdlib.h>

#include "ast.h"

// Allocate a new AST Node, and initialize it with the given type
ASTNode* ast_node_create(ASTNodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));

    // Initialize the created node, this implicitly checks for NULL
    if (ast_node_init(node, type) != 0) {
        free(node);  // free of NULL is a no-op
        return NULL;
    }

    return node;
}


// Initialize an AST Node
int ast_node_init(ASTNode* node, ASTNodeType type) {
    if (node == NULL) {
        return -1;
    }

    *node = (ASTNode){
        .type = type,
        .child1 = NULL,
        .extra = {0},
    };

    return 0;
}


// Release memory allocated for the given AST Node
void ast_node_free(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    if (node->type == CHAR_NODE) {
        free(node);
        return;
    }

    ast_node_free(node->child1);

    if (node->type == OR_NODE || node->type == CONCAT_NODE) {
        ast_node_free(node->extra.child2);
    }

    free(node);
}
