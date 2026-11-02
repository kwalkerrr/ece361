#include "lock.h"
#include <stdio.h>
#include <stdlib.h>

/* Runs one input character through the FSM and prints the transition.
 * Characters that are not events (such as whitespace) are skipped. */
static state_t handle_char(state_t s, char c) {
    int e = lock_event(s, c);
    action_t a = A_UNDEFINED;
    state_t next;

    if (e < 0)
        return s;

    next = lock_step(s, (event_t)e, &a);
    printf("%s %c %s -> %s %s\n", state_name(s), c, event_name((event_t)e),
           state_name(next), action_name(a));
    return next;
}

int main(void) {
    state_t s = LOCKED;
    int c;

    while ((c = getchar()) != EOF)
        s = handle_char(s, (char)c);

    printf("final: %s\n", state_name(s));
    return EXIT_SUCCESS;
}
