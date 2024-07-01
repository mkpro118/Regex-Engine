#ifndef REGEX_NFA_H
#define REGEX_NFA_H

#include "nfa_state.h"
#include <stdbool.h>

typedef struct NFA {
    NFAState* start_state;
    NFAStateList* final_states;
} NFA;

CREATE_LIST_TYPE_FOR(NFAState*, NFAStateSet)

/**
 * Create a heap allocated NFA
 * NFA = Non-deterministic Finite Automata
 *
 * @param  start_state  Starting state of this NFA
 * @param  final_states List of accepting states of this NFA
 *
 * @return A pointer to a heap allocated NFA on success,
 *         NULL on failure
 */
NFA* nfa_create(NFAState* start_state, NFAStateList* final_states);

/**
 * Initialize the given NFA
 *
 * @param  nfa          A pointer to the NFA to initialize
 * @param  start_state  Starting state of this NFA
 * @param  final_states List of accepting states of this NFA
 *
 * @return 0 on success, -1 on failure
 */
int nfa_init(NFA* nfa, NFAState* start_state, NFAStateList* final_states);

/**
 * Releases the memory used by the given NFA
 *
 * @param nfa The NFA to deallocate
 */
void nfa_free(NFA* nfa);

/**
 * Perform a regex match using the given NFA on the given string
 * 
 * @param  nfa    The NFA to match with
 * @param  string The string to match
 *
 * @return true if the string matches the grammar of the NFA, false otherwise
 */
bool nfa_match(NFA* nfa, const char* string);

#endif // REGEX_NFA_H
