#include "../lock.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 128

static int passed = 0;
static int failed = 0;

/* Written out independently of lock.c so lock_table is checked against the
 * intended design rather than against itself. */
static const transition_t expected_table[N_STATES][N_EVENTS] = {
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

static void report(bool ok, const char *expected, const char *actual) {
    if (ok)
        passed++;
    else
        failed++;

    printf("%s\nexpected: %s\nactual:   %s\n\n",
           ok ? "[PASS]" : "[FAIL]", expected, actual);
}

static void format_step(char *buf, state_t s, event_t e, transition_t t) {
    snprintf(buf, BUF_LEN, "lock_step(%s, %s) -> %s, %s",
             state_name(s), event_name(e),
             state_name(t.next), action_name(t.action));
}

static void test_step(state_t s, event_t e) {
    transition_t want = expected_table[s][e];
    transition_t got = {s, A_UNDEFINED};
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    got.next = lock_step(s, e, &got.action);
    format_step(want_str, s, e, want);
    format_step(got_str, s, e, got);
    report(want.next == got.next && want.action == got.action,
           want_str, got_str);
}

static void test_table(void) {
    for (state_t s = 0; s < N_STATES; s++)
        for (event_t e = 0; e < N_EVENTS; e++)
            test_step(s, e);
}

static void format_event(char *buf, state_t s, char c, int e) {
    snprintf(buf, BUF_LEN, "lock_event(%s, '%c') -> %s (%d)",
             state_name(s), c, event_name((event_t)e), e);
}

static void test_event(state_t s, char c, int want) {
    int got = lock_event(s, c);
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    format_event(want_str, s, c, want);
    format_event(got_str, s, c, got);
    report(want == got, want_str, got_str);
}

static void test_events(void) {
    /* Each wrong digit is one that is correct in some other state. */
    test_event(LOCKED, '3', E_RIGHT);
    test_event(LOCKED, '1', E_WRONG);
    test_event(GOT1, '1', E_RIGHT);
    test_event(GOT1, '4', E_WRONG);
    test_event(GOT2, '4', E_RIGHT);
    test_event(GOT2, '3', E_WRONG);

    /* The code is already complete while open, so even its first digit is
     * E_WRONG. */
    test_event(OPEN, '3', E_WRONG);
    test_event(OPEN, '9', E_WRONG);

    test_event(LOCKED, 'c', E_CLOSE);
    test_event(LOCKED, 't', E_TIMEOUT);
    test_event(LOCKED, 'x', -1);
}

/* Feeds each character of `seq` to the FSM starting from LOCKED. Characters
 * that are not events (such as spaces) are skipped. */
static state_t run_sequence(const char *seq, action_t *last) {
    state_t s = LOCKED;

    *last = A_UNDEFINED;
    for (const char *p = seq; *p != '\0'; p++) {
        int e = lock_event(s, *p);
        if (e >= 0)
            s = lock_step(s, (event_t)e, last);
    }
    return s;
}

static void format_sequence(char *buf, const char *seq, state_t s, action_t a) {
    snprintf(buf, BUF_LEN, "\"%s\" -> %s, %s",
             seq, state_name(s), action_name(a));
}

static void test_sequence(const char *seq, state_t want_s, action_t want_a) {
    action_t got_a;
    state_t got_s = run_sequence(seq, &got_a);
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    format_sequence(want_str, seq, want_s, want_a);
    format_sequence(got_str, seq, got_s, got_a);
    report(want_s == got_s && want_a == got_a, want_str, got_str);
}

static void test_sequences(void) {
    test_sequence("3 1 4", OPEN, A_UNLOCK);
    test_sequence("3 3 1 4", LOCKED, A_BEEP);
    test_sequence("3 1 t 4", LOCKED, A_BEEP);
    test_sequence("3 1 4 c", LOCKED, A_LOCK);
}

int main(void) {
    test_table();
    test_events();
    test_sequences();

    printf("%d passed, %d failed\n", passed, failed);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
