#ifndef REGEX_STATE_H
#define REGEX_STATE_H

#include <sys/types.h>
#include <stdbool.h>

// There are 95 printable characters, +1 for epsilon transition
// Technically, we would only support 89 printable characters,
// since 6 of them are used for regex control, which are
// *, +, ?, |, (, )
// However hashing strategy is easier if we reserve 96 spots
// So, if we do the math, we waste
// 6 * sizeof(NFAStateList) or,
// 6 * sizeof(2 * sizeof(size_t) + sizeof(NFAState*)) bytes per NFAState
// On my 64 bit machine, this is
// 6 * (2 * 8 + 8) = 6 * 24 = 144 bytes.
// It is what it is, a more complex hashing strategy will consume more time
// Tradeoffs...
#define MAX_N_TRANSITIONS 96

typedef struct NFAStateList NFAStateList;

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
    NFAStateList* transitions[MAX_N_TRANSITIONS];
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

/**
 * Get the transition for the `on` character for the given NFA State
 *
 * @param  from State to transition from
 * @param  on   The character on which to transition
 *
 * @return A pointer to the list of transition state if it exists,
 *         NULL if it doesn't
 */
NFAStateList* get_transition(NFAState* from, char on);

#endif // REGEX_STATE_H
