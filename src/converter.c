#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

#define FREE_NFA(nfa) do {\
/* This is the Release NFA memory on error */\
nfa_free(nfa);\
} while(0);

#define FREE_NFAS(nfa1, nfa2) FREE_NFA((nfa1)); FREE_NFA((nfa2));

#define NFA_CREATE do {\
/* Create list of final states,only one state here */\
if (NFAStateList_add(final_states, &final) < 0) {\
    return free_resources(start, final, final_states);\
}\
nfa = nfa_create(start, final_states);\
} while(0)

#define NFA_PARTIAL_FREE(nfa) do {\
NFAStateList_free((nfa)->final_states, NULL);\
free((nfa)->final_states);\
free((nfa));\
} while(0);

// 3/6 nodes have repeated code to create a nfa and release child resources.
// Fill it in with a macro to keep code dry
#define NFA_CREATE_SINGLE_CHILD do {\
NFA_CREATE;\
NFA_PARTIAL_FREE(child_nfa);\
} while(0)

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

static
int epsilon_from_child_final_states(NFA* child_nfa, State final, bool to_child) {
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
        // to our final state.
        if (add_transition(state, final, EPSILON) < 0) {
            return -1;
        }

        if (to_child) {
            // Every final state of the child nfa can epsilon transition
            // back to the child's start state for 1 or more repetitions.
            // This is the property of the (*) and (+) metacharacters in regex.
            if (add_transition(state, child_nfa->start_state, EPSILON) < 0) {
                return -1;
            }
        }
    }

    return 0;
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
    NFA* left_nfa = NULL;
    NFA* right_nfa = NULL;

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
    The idea for the next three nodes, i.e. nodes that have one child, is,
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

        // Add link to parent NFA's start state. Zero repetition case
        if (add_transition(start, final, EPSILON) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        // Add link to child NFA's start state
        if (add_transition(start, child_nfa->start_state, EPSILON) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transitions from child's final states to our final state
        if (epsilon_from_child_final_states(child_nfa, final, true) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        NFA_CREATE_SINGLE_CHILD;
        break;

    case PLUS_NODE:
        child_nfa = convert_ast_to_nfa(root->child1);
        if (child_nfa == NULL) {
            return free_resources(start, final, final_states);
        }

        // Note: No link to parent NFA's start state, the (+) metacharacter
        // requires at least 1 occurence of the child pattern

        // Add link to child NFA's start state
        if (add_transition(start, child_nfa->start_state, EPSILON) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transitions from child's final states to our final state
        if (epsilon_from_child_final_states(child_nfa, final, true) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        NFA_CREATE_SINGLE_CHILD;
        break;

    case QUESTION_NODE:
        child_nfa = convert_ast_to_nfa(root->child1);
        if (child_nfa == NULL) {
            return free_resources(start, final, final_states);
        }

        // Add link to parent NFA's start state. Zero repetition case
        if (add_transition(start, final, EPSILON) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        // Add link to child NFA's start state
        if (add_transition(start, child_nfa->start_state, EPSILON) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transitions from child's final states to our final state
        // But not to the child itself, the (?) metacharacter requires
        // zero or one occurences only
        if (epsilon_from_child_final_states(child_nfa, final, false) < 0) {
            FREE_NFA(child_nfa);
            return free_resources(start, final, final_states);
        }

        NFA_CREATE_SINGLE_CHILD;
        break;

////////////////////////////////////////////////////////////////////////////////
/*
    The next two nodes have two children. These children ASTs are still
    independent, so we can still create NFA for them independently.

    The strategy for the OR node is to
    1. Add epsilon transitions from parent NFA's start state to
       the start state for both the left and right child NFAs.
    2. Add epsilon transitions to the final state from all
       of the final states of both the left and right child NFAs.
*/
////////////////////////////////////////////////////////////////////////////////

    case OR_NODE:
        left_nfa = convert_ast_to_nfa(root->child1);
        if (left_nfa == NULL) {
            return free_resources(start, final, final_states);
        }

        right_nfa = convert_ast_to_nfa(root->extra.child2);
        if (right_nfa == NULL) {
            FREE_NFA(left_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transition from parent's start to left child's start
        if (add_transition(start, left_nfa->start_state, EPSILON) < 0) {
            FREE_NFAS(left_nfa, right_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transition from parent's start to right child's start
        if (add_transition(start, right_nfa->start_state, EPSILON) < 0) {
            FREE_NFAS(left_nfa, right_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transition from left child's final states to
        // parent's final. Do not link from final to start in the child
        if (epsilon_from_child_final_states(left_nfa, final, false) < 0) {
            FREE_NFAS(left_nfa, right_nfa);
            return free_resources(start, final, final_states);
        }

        // Add epsilon transition from right child's final states to
        // parent's final. Do not link from final to start in the child
        if (epsilon_from_child_final_states(right_nfa, final, false) < 0) {
            FREE_NFAS(left_nfa, right_nfa);
            return free_resources(start, final, final_states);
        }

        NFA_CREATE;
        NFA_PARTIAL_FREE(left_nfa);
        NFA_PARTIAL_FREE(right_nfa);
        break;

    case CONCAT_NODE:
        break;

    default:
        // Ideally should never get here
        return free_resources(start, final, final_states);
    }

    if (nfa == NULL) {
        return free_resources(start, final, final_states);
    }

    return nfa;
}
