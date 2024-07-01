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

void nfa_free(NFA* nfa) {
    if (nfa == NULL) {
        return;
    }
    if (nfa->start_state != NULL) {
        state_free(nfa->start_state);
    }

    if (nfa->final_states != NULL) {
        NFAStateList_free(nfa->final_states, state_free);
    }
}

bool nfa_match(NFA* nfa, const char* string) {
    if (nfa == NULL || string == NULL) {
        return false;
    }

    NFAStateSet* current_states = NFAStateSet_create(1);
    NFAStateSet_add(current_states, &nfa->start_state);

    for (size_t i = 0; i < strlen(string); i++) {
        char c = string[i];
        NFAStateSet* next_states = NFAStateSet_create(1);

        for (size_t j = 0; j < current_states->size; j++) {
            NFAState* state = current_states->list[j];
            NFAStateList* list = get_transition(state, c);
            if (list != NULL) {
                for (size_t k = 0; k < list->size; k++) {
                    NFAState* next_state = &list->list[k];
                    NFAStateSet_add(next_states, &next_state);
                }
            }
        }

        NFAStateSet_free(current_states, NULL);
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
