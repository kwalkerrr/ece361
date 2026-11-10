#ifndef HW_PANEL_H
#define HW_PANEL_H

#include <stdint.h>

/**
 * Finds the switches that went from off to on between ticks. One bit is one
 * switch.
 * @param now The switch register right now.
 * @param before The switch register from the previous tick.
 * @return A 32-bit register with 1s in the positions of the switches that went
 * from off to on between ticks.
 */
uint32_t panel_pressed(uint32_t now, uint32_t before);

/**
 * Finds the switches that went from on to off between ticks. One bit is one
 * switch.
 * @param now The switch register right now.
 * @param before The switch register from the previous tick.
 * @return A 32-bit register with 1s in the positions of the switches that went
 * from on to off between ticks.
 */
uint32_t panel_released(uint32_t now, uint32_t before);

/**
 * Turns LED `n` on or off by a read-modify-write process so every other LED in
 * the register keeps its value. If `n` does not satisfy 0 <= `n` <= 31, the
 * function prints to `stderr` and returns without modifying any values. The
 * same applies if `base` is `NULL`.
 * @param base The base address of the iom361. LEDS_REG (see iom361_r4.h) is
 * used as an offset to access the LEDs register.
 * @param n The LED to turn on (0 <= `n` <= 31). Note that this may modify LEDs
 * beyond the number that exist from the iom361 initialization; use with
 * caution.
 * @param on 0 turns the LED off. Any non-zero value turns the LED on.
 * @return -1 if `n` is not between 0 and 31 or `base` is `NULL`. The return
 * code of an iom361 function if it returns non-zero. 0 otherwise.
 */
int panel_led(uint32_t *base, int n, int on);

/**
 * Toggles LED `n` by a read-modify-write process so every other LED in the
 * register keeps its value. If `n` does not satisfy 0 <= `n` <= 31, the
 * function prints to `stderr` and returns without modifying any values. The
 * same applies if `base` is `NULL`.
 * @param base The base address of the iom361. LEDS_REG (see iom361_r4.h) is
 * used as an offset to access the LEDs register.
 * @param n The LED to toggle (0 <= `n` <= 31). Note that this may modify LEDs
 * beyond the number that exist from the iom361 initialization; use with
 * caution.
 * @return -1 if `n` is not between 0 and 31 or `base` is `NULL`. The return
 * code of an iom361 function if it returns non-zero. 0 otherwise.
 */
int panel_toggle(uint32_t *base, int n);

#endif
