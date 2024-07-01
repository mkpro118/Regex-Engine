#include "nfa_state.h"
#include "list.h"

/**
 * Collection of transition states
 *
 * Members
 *    - capacity: Cacpacity of this collection, i.e, max elements it can hold
 *    - size: Size of this collection, i.e, number of elements it currently has
 *    - list: The underlying list of transition states
 */
CREATE_LIST_FOR(NFAState, NFAStateList)

// Create and initialize a heap allocated NFA State
NFAState* state_create(bool is_final) {
    NFAState* state = malloc(sizeof(NFAState));
    if (state == NULL) {
        return NULL;
    }

    if (state_init(state, is_final) != 0) {
        free(state);
        return NULL;
    }

    return state;
}

// Initialize the given NFA State
int state_init(NFAState* state, bool is_final) {
    if (state == NULL) {
        return -1;
    }

    state->is_final = is_final;

    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        state->transitions[i] = NULL;
    }

    return 0;
}

// Release the memory used by this NFA State.
void state_free(NFAState* state) {
    if (state == NULL) {
        return;
    }

    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        if (state->transitions[i] != NULL) {
            NFAStateList_free(state->transitions[i], state_free);
            free(state->transitions[i]);
        }
        state->transitions[i] = NULL;
    }
}

static inline int char_hash(char c) {
    return c == 0 ? c : (int) (c - 0x1F);
}

// Add a link between the `from` and `to` state for the `on` character.
int add_transition(NFAState* from, NFAState* to, char on) {
    if (from == NULL || to == NULL) {
        return -1;
    }

    // Calculate the index for the transition
    int index = char_hash(on);
    if (index < 0 || index >= MAX_N_TRANSITIONS) {
        return -1;  // Invalid character
    }

    // If the transition list doesn't exist, create it
    if (from->transitions[index] == NULL) {
        from->transitions[index] = NFAStateList_create(1);
        if (from->transitions[index] == NULL) {
            return -1;
        }
    }

    // Add the transition
    int size = NFAStateList_add(from->transitions[index], to);
    return size;
}

// Get the transition for the `on` character for the given NFA State
NFAStateList* get_transition(NFAState* from, char on) {
    if (from == NULL) {
        return NULL;
    }

    int index = char_hash(on);
    if (index < 0 || index >= MAX_N_TRANSITIONS) {
        return NULL;  // Invalid character
    }

    return from->transitions[index];
}
