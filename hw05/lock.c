#include "lock.h"

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