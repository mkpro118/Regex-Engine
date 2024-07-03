#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

typedef NFAState* State;
typedef NFAStateList* StateList;

static inline
void* free_resources(State start, State final, StateList list) {
    if (start != NULL) {
        state_free(start);
    }

    if (final != NULL) {
        state_free(final);
    }

    if (list != NULL) {
        NFAStateList_free(list, NULL);
        free(list);
    }
    return NULL;
}

// Convert AST to NFA
NFA* convert_ast_to_nfa(ASTNode* root) {
    State start = state_create(false);
    if (start == NULL) {
        return NULL;
    }

    State final = state_create(true);
    if (final == NULL) {
        return free_resources(start, NULL, NULL);
    }

    StateList final_states = NFAStateList_create(1);
    if (final_states == NULL) {
        return free_resources(start, final, NULL);
    }

    NFA* nfa;

    switch (root->type) {
    case CHAR_NODE:
        // add a transition on the node's character
        if (add_transition(start, final, root->extra.character) < 0) {
            return free_resources(start, final, final_states);
        }

        // Create list of final states,only one state here
        if (NFAStateList_add(final_states, &final) < 0) {
            return free_resources(start, final, final_states);
        }

        // Create the NFA
        nfa = nfa_create(start, final_states);
        if (nfa == NULL) {
            return free_resources(start, final, final_states);
        }

        return nfa;

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
