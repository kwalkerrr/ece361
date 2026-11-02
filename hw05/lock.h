#ifndef HW_LOCK_H
#define HW_LOCK_H

typedef enum {
    LOCKED,
    GOT1,
    GOT2,
    OPEN,
    N_STATES
} state_t;

typedef enum {
    E_RIGHT,
    E_WRONG,
    E_CLOSE,
    E_TIMEOUT,
    N_EVENTS
} event_t;

typedef enum {
    A_UNDEFINED,
    A_NONE,
    A_UNLOCK,
    A_LOCK,
    A_BEEP,
    N_ACTIONS
} action_t;

typedef struct {
    state_t next;
    action_t action;
} transition_t;

extern const transition_t lock_table[N_STATES][N_EVENTS];

/**
 * Steps to the next state and determines the appropriate action based on the
 * transition table indexed by the current state and event. If `s`, `e`, or
 * `action` contain invalid values (see below), an error is printed to stderr
 * and the function returns early with the current state.
 * @param s The current state of the FSM. 0 <= `s` < N_STATES.
 * @param e The event to which to react. 0 <= `e` < N_EVENTS.
 * @param action The address at which to store the resultant action. Must not be
 * NULL.
 * @return If `s` or `e` are out of bounds or `action` is NULL, the current
 * state. Otherwise, the next state.
 */
state_t lock_step(state_t s, event_t e, action_t *action);

/**
 * Determines the event for the current state depending on the input character.
 * A digit is E_RIGHT for the correct next digit of the code and E_WRONG
 * otherwise. 'c' is E_CLOSE (the door closing), and 't' is E_TIMEOUT (the timer
 * for typing in the code running out).
 * @param s The current state of the FSM. 0 <= `s` < N_STATES.
 * @param c The character to read to determine the event.
 * @return If `s` is out of bounds, -2. If `c` is a digit, a 'c', or a 't', the
 * integer value that corresponds with the event given in the description in the
 * event_t enum. Otherwise, -1.
 */
int lock_event(state_t s, char c);

/**
 * Gets the string equivalent of a state.
 * @param s The state of which to get a string. 0 <= `s` < N_STATES.
 * @return If `s` is within bounds, the string equivalent of the state's name
 * (prefix stripped). Otherwise, "?".
 */
const char *state_name(state_t s);

/**
 * Gets the string equivalent of an event.
 * @param e The event of which to get a string. 0 <= `e` < N_EVENTS.
 * @return If `e` is within bounds, the string equivalent of the event's name
 * (prefix stripped). Otherwise, "?".
 */
const char *event_name(event_t e);

/**
 * Gets the string equivalent of an action.
 * @param a The action of which to get a string. 0 <= `a` < N_ACTIONS.
 * @return If `a` is within bounds, the string equivalent of the action's name
 * (prefix stripped). Otherwise, "?".
 */
const char *action_name(action_t a);

#endif