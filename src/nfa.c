#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "nfa.h"

CREATE_LIST_IMPL_FOR(NFAState*, NFAStateSet)

NFA* nfa_create(NFAState* start_state, NFAStateList* final_states) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    nfa->start_state = start_state;
    nfa->final_states = final_states;
    return nfa;
}

/**
 * Gathers all the states from a given start state
 *
 * Uses depth first search on the state's transitions to gather states
 * I am okay with using recursion here for simplicity.
 *
 * @param list  The list to place the gathered states in
 * @param state The state to start the search from
 */
void gather_states(NFAStateList* list, NFAState* state) {
    if (list == NULL || state == NULL) {
        return;
    }

    // Set like add
    if (state->visited) {
        return;
    }

    NFAStateList_add(list, &state);
    state->visited = true;

    // Loop over the transition characters
    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        if (state->transitions[i] == NULL) {
            continue;
        }

        // Loop over the possible transitions on a given characters
        for (size_t j = 0; j < state->transitions[i]->size; j++) {
            gather_states(list, state->transitions[i]->list[j]);
        }
    }
}

void nfa_free(NFA* nfa) {
    if (nfa == NULL) {
        return;
    }

    // there are at least start_state + len(final_state) states
    // Initialize the list with that capacity
    NFAStateList* gathered_states = NFAStateList_create(1 + nfa->final_states->size);
    gather_states(gathered_states, nfa->start_state);

    // Free all the gathered states
    NFAStateList_free(gathered_states, state_ptr_free);
    free(gathered_states);

    NFAStateList_free(nfa->final_states, NULL);
    free(nfa->final_states);
}

int state_ptr_cmp(const NFAState** a, const NFAState** b) {
    return ((*a)->ID - (*b)->ID);
}

// Helper function to perform epsilon closure
static void epsilon_closure(NFAStateSet* current_states, NFAStateSet* next_states) {
    for (size_t i = 0; i < current_states->size; i++) {
        NFAState* state = current_states->list[i];
        NFAStateList* epsilon_transitions = get_transition(state, '\0');

        if (epsilon_transitions != NULL) {
            for (size_t j = 0; j < epsilon_transitions->size; j++) {
                NFAState* epsilon_state = epsilon_transitions->list[j];
                if (NFAStateSet_find(next_states, (const NFAState**)&epsilon_state, state_ptr_cmp) == NULL) {
                    NFAStateSet_add(next_states, &epsilon_state);
                    epsilon_closure(next_states, next_states);
                }
            }
        }
    }
}

bool nfa_match(NFA* nfa, const char* string) {
    if (nfa == NULL || string == NULL) {
        return false;
    }

    NFAStateSet* current_states = NFAStateSet_create(10);
    NFAStateSet* next_states = NFAStateSet_create(10);

    if (current_states == NULL || next_states == NULL) {
        NFAStateSet_free(current_states, NULL);
        NFAStateSet_free(next_states, NULL);
        return false;
    }

    // Add initial state and perform epsilon closure
    NFAStateSet_add(current_states, &nfa->start_state);
    epsilon_closure(current_states, current_states);

    // Process each character in the input string
    for (size_t i = 0; i < strlen(string); i++) {
        char c = string[i];

        // For each current state, find all possible next states
        for (size_t j = 0; j < current_states->size; j++) {
            NFAState* state = current_states->list[j];
            NFAStateList* transitions = get_transition(state, c);

            if (transitions != NULL) {
                for (size_t k = 0; k < transitions->size; k++) {
                    NFAState* next_state = transitions->list[k];
                    if (NFAStateSet_find(next_states, (const NFAState**)&next_state, state_ptr_cmp) == NULL) {
                        NFAStateSet_add(next_states, &next_state);
                    }
                }
            }
        }

        // Perform epsilon closure on next_states
        epsilon_closure(next_states, next_states);

        // Swap current_states and next_states, clear next_states
        NFAStateSet* temp = current_states;
        current_states = next_states;
        next_states = temp;
        next_states->size = 0;  // Clear next_states for the next iteration
    }

    // Check if any of the current states is a final state
    bool match = false;
    for (size_t i = 0; i < current_states->size; i++) {
        NFAState* state = current_states->list[i];
        if (state->is_final) {
            match = true;
            break;
        }
    }

    // Clean up
    NFAStateSet_free(current_states, NULL);
    free(current_states);
    NFAStateSet_free(next_states, NULL);
    free(next_states);

    return match;
}
