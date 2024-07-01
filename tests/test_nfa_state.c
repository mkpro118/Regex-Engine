#include <stdbool.h>

#define FAIL_FAST

#include "testlib/asserts.h"
#include "testlib/tests.h"

#include "nfa_state.h"

NFAState* state_create(bool is_final);
int state_init(NFAState* state, bool is_final);
void state_free(NFAState* state);
int add_transition(NFAState* from, NFAState* to, char on);

NFAState state;

int test_state_create(void) {
    TEST_BEGIN;

    // Case 1: Accepting/Final state
    {
        NFAState* state = state_create(true);
        assert_is_not_null(state);

        assert_equals_int(state->is_final, true);
        assert_is_not_null(state->transitions);

        state_free(state);
        free(state);
    }

    // Case 2: Non-Accepting/Non-Final state
    {
        NFAState* state = state_create(false);
        assert_is_not_null(state);

        assert_equals_int(state->is_final, false);
        assert_is_not_null(state->transitions);

        state_free(state);
        free(state);
    }

    TEST_END;
}

int test_state_init(void) {
    TEST_BEGIN;

    // Case 1: Accepting/Final state
    {
        int ret = state_init(&state, true);
        assert_equals_int(ret, 0);

        assert_equals_int(state.is_final, true);
        assert_is_not_null(state.transitions);

        state_free(state);
    }

    // Case 2: Non-Accepting/Non-Final state
    {
        int ret = state_init(&state, false);
        assert_equals_int(ret, 0);

        assert_equals_int(state.is_final, false);
        assert_is_not_null(state.transitions);

        state_free(state);
    }

    TEST_END;
}

int test_add_transition(void) {
    TEST_BEGIN;

    TEST_END;
}


Test tests[] = {
    {.name="test_state_create", .func=test_state_create},
    {.name="test_state_init", .func=test_state_init},
    {.name="test_add_transition", .func=test_add_transition},
    {.name=NULL},
};


int main(int argc, char const *argv[]) {
    return default_main(&argv[1], argc - 1);
}
