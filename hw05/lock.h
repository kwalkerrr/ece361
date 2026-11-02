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
 * transition table indexed by the current state and event.
 * @param s The current state of the FSM.
 * @param e The event to react to.
 * @param action The address at which to store the resultant action.
 * @return The next state.
 */
state_t lock_step(state_t s, event_t e, action_t *action);

/**
 * Determines the event for the current state depending on the input character.
 * A digit is E_RIGHT for the correct next digit of the code and E_WRONG
 * otherwise. 'c' is E_CLOSE (the door closing), and 't' is E_TIMEOUT
 * (the timer for typing in the code running out).
 * @param s The current state of the FSM.
 * @param c The character to read to determine the event.
 * @return If `c` is a digit, a 'c', or a 't', the integer value that
 * corresponds with the event given in the description in the event_t enum.
 * Otherwise, -1.
 */
int lock_event(state_t s, char c);

/**
 * Gets the string equivalent of a state.
 * @param s The state of which to get a string.
 * @return If `s` is within the valid range for state_t (0 <= s < N_STATES), the
 * string equivalent of the state's name. Otherwise, "?".
 */
const char *state_name(state_t s);

/**
 * Gets the string equivalent of an event.
 * @param e The event of which to get a string.
 * @return If `e` is within the valid range for event_t (0 <= e < N_EVENTS), the
 * string equivalent of the event's name. Otherwise, "?".
 */
const char *event_name(event_t e);

/**
 * Gets the string equivalent of an action.
 * @param a The action of which to get a string.
 * @return If `a` is within the valid range for action_t (0 <= a < N_ACTIONS),
 * the string equivalent of the action's name. Otherwise, "?".
 */
const char *action_name(action_t a);

#endif