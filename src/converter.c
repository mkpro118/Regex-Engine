#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

// Convert AST to NFA
NFA* convert_ast_to_nfa(ASTNode* root) {
    NFAState* start;
    NFAState* final;
    NFAStateList* final_states = NFAStateList_create(1);

    switch (root->type) {
    case CHAR_NODE:
        // Create states
        start = state_create(false);
        final = state_create(true);

        // add a transition on the node's character
        add_transition(start, final, root->extra.character);

        // Create list of final states,only one state here
        NFAStateList_add(final_states, &final);

        // Create the NFA
        return nfa_create(start, final_states);

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
    return NULL;
}
