#ifndef REGEX_STATE_H
#define REGEX_STATE_H

#include <stdbool.h>
#define MAX_N_TRANSITIONS 128

/**
 * Represents a state in the NFA
 * NFA stands for Non-deterministic Finite Automata
 *
 * Members
 *    - is_final: Whether or not it is an accepting/final state
 *    - transitions: A link to the next state on a given character
 */
typedef struct NFAState {
    bool is_final;
    struct NFAState* transitions[MAX_N_TRANSITIONS];
} NFAState;


/**
 * Create and initialize a heap allocated NFA State
 *
 * @param  is_final true means the state is an accepting state, false otherwise
 *
 * @return A pointer to a heap allocated NFAState on success,
 *         NULL on failure
 */
NFAState* state_create(bool is_final);


/**
 * Initialize the given NFA State
 *
 * @param  state    The state to initialize
 * @param  is_final true means the state is an accepting state, false otherwise
 *
 * @return  0 on success, -1 on failure
 */
int state_init(NFAState* state, bool is_final);


/**
 * Release the memory used by this NFA State.
 *
 * @param state The state to deallocate
 */
void state_free(NFAState* state);


/**
 * Add a link between the `from` state and the `to` state
 * for the `on` character.
 *
 * @param  from State to transition from
 * @param  to   State to transition to
 * @param  on   The character on which to transition
 *
 * @return 0 on success, -1 on failure
 */
int add_transition(NFAState* from, NFAState* to, char on);

#endif // REGEX_STATE_H
