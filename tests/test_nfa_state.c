#define FAIL_FAST
#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "list.h"
#include "nfa_state.h"

CREATE_LIST_IMPL_FOR(NFAState*, NFAStateList)

// Global NFAState variable
NFAState state;


int test_state_init() {

    TEST_BEGIN;

    int ret = state_init(&state, false);
    assert_equals_int(ret, 0);
    assert_equals_int(state.is_final, false);

    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        assert_is_null(state.transitions[i]);
    }

    state_free(&state);
    TEST_END;
}

int test_add_transition() {

    TEST_BEGIN;
    NFAState to;
    state_init(&to, true);

    int n_transitions = add_transition(&state, &to, 'a');
    assert_equals_int(n_transitions, 1);

    NFAStateList* list = get_transition(&state, 'a');
    assert_is_not_null(list);

    int size = NFAStateList_size(get_transition(&state, 'a'));
    assert_equals_int(size, 1);

    // Add another transition on the same character
    NFAState to2;
    state_init(&to2, false);

    n_transitions = add_transition(&state, &to2, 'a');
    assert_equals_int(n_transitions, 2);
    size = NFAStateList_size(get_transition(&state, 'a'));
    assert_equals_int(size, 2);

    // Test invalid transitions
    n_transitions = add_transition(NULL, &to, 'b');
    assert_equals_int(n_transitions, -1);
    n_transitions = add_transition(&state, NULL, 'b');
    assert_equals_int(n_transitions, -1);
    n_transitions = add_transition(&state, &to, '\n');
    assert_equals_int(n_transitions, -1);

    // Clean up transitions
    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        if (state.transitions[i] != NULL) {
            NFAStateList_free(state.transitions[i], NULL);
            free(state.transitions[i]);
            state.transitions[i] = NULL;
        }
    }

    state_free(&state);
    TEST_END;
}

int test_state_free() {
    TEST_BEGIN;
    NFAState to1, to2;
    state_init(&state, false);
    state_init(&to1, false);
    state_init(&to2, false);

    add_transition(&state, &to1, 'a');
    add_transition(&state, &to2, 'b');

    // This should free all allocated memory without crashing
    state_free(&state);

    // Verify that transitions are freed
    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        assert_is_null(state.transitions[i]);
    }

    TEST_END;
}

Test tests[] = {
    {.name="test_state_init", .func=test_state_init},
    {.name="test_add_transition", .func=test_add_transition},
    {.name="test_state_free", .func=test_state_free},
    {.name=NULL, .func=NULL}
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
