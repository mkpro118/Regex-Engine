#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

// Convert AST to NFA
NFA* convert_ast_to_nfa(ASTNode* root) {
    switch (root->type) {
    case CHAR_NODE:
        break;

    case STAR_NODE:
        break;

    case PLUS_NODE:
        break;

    case QUESTION_NODE:
        break;

    case OR_NODE:
        break;

    case CONCAT_NODE:
        break;

    default:
        // Ideally should never get here
        return NULL;
    }
}
