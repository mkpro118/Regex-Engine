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
    NFAState* fptr = &final_state;
    NFAStateList_add(final_states, &fptr);

    nfa = nfa_create(&start_state, final_states);
    return 0;
}

int teardown() {
    state_free(&start_state);
    nfa_free(nfa);
    free(nfa->final_states);
    free(nfa);
    return 0;
}

int test_nfa_create() {
    TEST_BEGIN;
    assert_is_not_null(nfa);
    assert_equals_ptr(nfa->start_state, &start_state, NFAState*);
    assert_is_not_null(nfa->final_states);
    assert_equals_int(NFAStateList_size(nfa->final_states), 1);
    assert_equals_int(nfa->final_states->list[0]->ID, final_state.ID);
    TEST_END;
}

int test_nfa_match_positive() {
    TEST_BEGIN;

    bool match = nfa_match(nfa, "ab");
    assert_equals_int(match, true);

    TEST_END;
}

int test_nfa_match_negative() {
    TEST_BEGIN;

    bool match = nfa_match(nfa, "a");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "b");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "abc");
    assert_equals_int(match, false);

    match = nfa_match(nfa, "");
    assert_equals_int(match, false);

    TEST_END;
}

int test_nfa_match_edge_cases() {
    TEST_BEGIN;

    bool match = nfa_match(NULL, "ab");
    assert_equals_int(match, false);

    match = nfa_match(nfa, NULL);
    assert_equals_int(match, false);

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
