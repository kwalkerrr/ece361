#include "lock.h"
#include <stdio.h>

#define FIRST_DIGIT '3'
#define SECOND_DIGIT '1'
#define THIRD_DIGIT '4'

const transition_t lock_table[N_STATES][N_EVENTS] = {
    [LOCKED] = {
        [E_RIGHT] = {GOT1, A_NONE},
        [E_WRONG] = {LOCKED, A_BEEP},
        [E_CLOSE] = {LOCKED, A_NONE},
        [E_TIMEOUT] = {LOCKED, A_LOCK}
    },
    [GOT1] = {
        [E_RIGHT] = {GOT2, A_NONE},
        [E_WRONG] = {LOCKED, A_BEEP},
        [E_CLOSE] = {GOT1, A_NONE},
        [E_TIMEOUT] = {LOCKED, A_LOCK}
    },
    [GOT2] = {
        [E_RIGHT] = {OPEN, A_UNLOCK},
        [E_WRONG] = {LOCKED, A_BEEP},
        [E_CLOSE] = {GOT2, A_NONE},
        [E_TIMEOUT] = {LOCKED, A_LOCK}
    },
    [OPEN] = {
        [E_RIGHT] = {OPEN, A_NONE},
        [E_WRONG] = {OPEN, A_NONE},
        [E_CLOSE] = {LOCKED, A_LOCK},
        [E_TIMEOUT] = {LOCKED, A_LOCK}
    }
};

state_t lock_step(state_t s, event_t e, action_t *action) {
    if (NULL == action) {
        fprintf(stderr, "lock_step: action is NULL\n");
        return s;
    }
    else if (s < 0 || s >= N_STATES) {
        fprintf(stderr, "lock_step: s of %d is out of bounds\n", (int)s);
        return s;
    }
    else if (e < 0 || e >= N_EVENTS) {
        fprintf(stderr, "lock_step: e of %d is out of bounds\n", (int)e);
        return s;
    }

    *action = lock_table[s][e].action;
    return lock_table[s][e].next;
}

int lock_event(state_t s, char c) {
    if (s < 0 || s >= N_STATES) {
        fprintf(stderr, "lock_event: s of %d is out of bounds\n", (int)s);
        return -2;
    }

    if ('c' == c)
        return (int)E_CLOSE;
    else if ('t' == c)
        return (int)E_TIMEOUT;
    else if (c >= '0' && c <= '9') {
        if (LOCKED == s && FIRST_DIGIT == c)
            return (int)E_RIGHT;
        else if (GOT1 == s && SECOND_DIGIT == c)
            return (int)E_RIGHT;
        else if (GOT2 == s && THIRD_DIGIT == c)
            return (int)E_RIGHT;
        else
            return (int)E_WRONG;
    }
    else
        return -1;
}