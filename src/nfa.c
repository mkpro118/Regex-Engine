#include <stdlib.h>
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
    if (!state->visited) {
        NFAStateList_add(list, &state);
        state->visited = true;
    }

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

bool nfa_match(NFA* nfa, const char* string) {
    if (nfa == NULL || string == NULL) {
        return false;
    }

    NFAStateSet* current_states = NFAStateSet_create(1);
    NFAStateSet_add(current_states, &nfa->start_state);

    size_t len_str = strlen(string);
    for (size_t i = 0; i < len_str; i++) {
        char c = string[i];

        NFAStateSet* next_states = NFAStateSet_create(1);

        for (size_t j = 0; j < current_states->size; j++) {
            NFAState* state = current_states->list[j];

            NFAStateList* list = get_transition(state, c);

            if (list != NULL) { // Has transition for c

                for (size_t k = 0; k < list->size; k++) {
                    NFAState* next_state = list->list[k];
                    const NFAState** state_ptr = (const NFAState**) &next_state;
                    NFAState** existing =
                        NFAStateSet_find(next_states, state_ptr, state_ptr_cmp);

                    if (existing == NULL) {
                        NFAStateSet_add(next_states, &next_state);
                    }
                }
            }
        }

        NFAStateSet_free(current_states, NULL); // no internal free is needed
        free(current_states);
        current_states = next_states;
    }

    bool match = false;
    for (size_t i = 0; i < current_states->size; i++) {
        NFAState* state = current_states->list[i];
        if (state->is_final) {
            match = true;
            break;
        }
    }

    NFAStateSet_free(current_states, NULL);
    free(current_states);
    return match;
}
