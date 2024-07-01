#include <stdbool.h>

#include "testlib/asserts.h"
#include "testlib/tests.h"
#include "nfa.h"
#include "nfa_state.h"

NFAState start_state, final_state, intermediate_state;
NFA* nfa;

int setup() {
    state_init(&start_state, false);
    state_init(&final_state, true);
    state_init(&intermediate_state, false);

    add_transition(&start_state, &intermediate_state, 'a');
    add_transition(&intermediate_state, &final_state, 'b');

    NFAStateList* final_states = NFAStateList_create(1);
    NFAStateList_add(final_states, &final_state);

    nfa = nfa_create(&start_state, final_states);
    return 0;
}

int teardown() {
    for (int i = 0; i < MAX_N_TRANSITIONS; i++) {
        if (start_state.transitions[i] != NULL) {
            NFAStateList_free(start_state.transitions[i], NULL);
            free(start_state.transitions[i]);
        }
        if (intermediate_state.transitions[i] != NULL) {
            NFAStateList_free(intermediate_state.transitions[i], NULL);
            free(intermediate_state.transitions[i]);
        }
    }
    NFAStateList_free(nfa->final_states, NULL);
    free(nfa->final_states);
    nfa_free(nfa);
    return 0;
}

int test_nfa_create() {
    TEST_BEGIN;
    assert_is_not_null(nfa);
    assert_equals_ptr(nfa->start_state, &start_state, NFAState*);
    assert_is_not_null(nfa->final_states);
    assert_equals_int(NFAStateList_size(nfa->final_states), 1);
    assert_equals_ptr(nfa->final_states->list[0], &final_state, NFAState*);
    TEST_END;
}

int test_nfa_match_positive() {
    TEST_BEGIN;
    assert_equals_int(nfa_match(nfa, "ab"), true);
    TEST_END;
}

int test_nfa_match_negative() {
    TEST_BEGIN;
    assert_equals_int(nfa_match(nfa, "a"), false);
    assert_equals_int(nfa_match(nfa, "b"), false);
    assert_equals_int(nfa_match(nfa, "abc"), false);
    assert_equals_int(nfa_match(nfa, ""), false);
    TEST_END;
}

int test_nfa_match_edge_cases() {
    TEST_BEGIN;
    assert_equals_int(nfa_match(NULL, "ab"), false);
    assert_equals_int(nfa_match(nfa, NULL), false);
    TEST_END;
}

Test tests[] = {
    {.name="test_nfa_create", .func=test_nfa_create},
    {.name="test_nfa_match_positive", .func=test_nfa_match_positive},
    {.name="test_nfa_match_negative", .func=test_nfa_match_negative},
    {.name="test_nfa_match_edge_cases", .func=test_nfa_match_edge_cases},
    {.name=NULL, .func=NULL}
};

int main(int argc, char* argv[]) {
    setup();
    int result = default_main(&argv[1], argc - 1);
    teardown();
    return result;
}
