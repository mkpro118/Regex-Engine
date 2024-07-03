#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

typedef NFAState* State;
typedef NFAStateList* StateList;

static const char EPSILON = '\0';

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
    if (root == NULL) {
        return NULL;
    }

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

    NFA* nfa = NULL;
    NFA* child_nfa = NULL;

    switch (root->type) {
////////////////////////////////////////////////////////////////////////////////
/*
    CharNode is the simplest of AST nodes to convert. It is a leaf node in the
    AST, and has no children.

    Essentially a char node is a transition from a start state to a final state
    on the character in the node. That's it!
*/
////////////////////////////////////////////////////////////////////////////////
    case CHAR_NODE:
        // Add a transition on the node's character
        if (add_transition(start, final, root->extra.character) < 0) {
            return free_resources(start, final, final_states);
        }

        // Create list of final states,only one state here
        if (NFAStateList_add(final_states, &final) < 0) {
            return free_resources(start, final, final_states);
        }

        // Create the NFA
        nfa = nfa_create(start, final_states);
        break;

////////////////////////////////////////////////////////////////////////////////
/*
    The idea for the rest of these nodes, i.e. nodes that have children, is,
    1. The child is independent of the current node. So, we can make
       an NFA for the child AST.
    2. Then wrap that NFA within our NFA, by adding relevant transitions.

    The process is somewhat like the decorator pattern..

    We use the states created by the "child" NFA, but discard the NFA itself.
    References to all the states are still maintained via transitions from
    the parent NFA. Cleanup of these references is done via a DFS search of
    states through the transitions.

    Our tests rigorously verify using ASan and Valgrind that no references
    are lost, and no memory is leaked, directly or indirectly.
*/
////////////////////////////////////////////////////////////////////////////////

    case STAR_NODE:
        child_nfa = convert_ast_to_nfa(root->child1);
        if (child_nfa == NULL) {
            return free_resources(start, final, final_states);
        }

        if (add_transition(start, final, EPSILON) < 0) {
            return free_resources(start, final, final_states);
        }

        if (add_transition(start, child_nfa->start_state, EPSILON) < 0) {
            return free_resources(start, final, final_states);
        }

        StateList child_final_states = child_nfa->final_states;
        for (size_t i = 0; i < child_final_states->size; i++) {
            State state = child_final_states->list[i];
            if (state == NULL) {
                continue;
            }

            // We wrap the child nfa, so it's final state is no longer a
            // final state
            state->is_final = false;

            // Every final state of the child nfa can epsilon transition
            // to our final state. The zero repetiton case
            // This is the property of the (*) metacharacter in regex.
            if (add_transition(state, final, EPSILON) < 0) {
                return free_resources(start, final, final_states);
            }

            // Every final state of the child nfa can epsilon transition
            // back to the child's start state for 1 or more repetitions.
            // This is the property of the (*) metacharacter in regex.
            if (add_transition(state, child_nfa->start_state, EPSILON) < 0) {
                return free_resources(start, final, final_states);
            }
        }

        // Create list of final states,only one state here
        if (NFAStateList_add(final_states, &final) < 0) {
            return free_resources(start, final, final_states);
        }
        nfa = nfa_create(start, final_states);

        // Release child NFA memory
        NFAStateList_free(child_nfa->final_states, NULL);
        free(child_nfa->final_states);
        free(child_nfa);
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

    if (nfa == NULL) {
        return free_resources(start, final, final_states);
    }

    return nfa;
}
