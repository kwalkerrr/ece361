#include "panel.h"
#include "iom361_r4.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PANEL_MAX_LED 31

uint32_t panel_pressed(uint32_t now, uint32_t before) {
    return now & ~before;
}

uint32_t panel_released(uint32_t now, uint32_t before) {
    return ~now & before;
}

static int panel_check_args(const char *caller, uint32_t *base, int n) {
    if (NULL == base) {
        fprintf(stderr, "%s: base is NULL\n", caller);
        return -1;
    }
    if (n < 0 || n > PANEL_MAX_LED) {
        fprintf(stderr, "%s: LED %d is outside 0 to %d\n", caller, n,
            PANEL_MAX_LED);
        return -1;
    }
    return 0;
}

static int panel_read_leds(const char *caller, uint32_t *base,
    uint32_t *leds) {
    int return_code;
    *leds = iom361_readReg(base, LEDS_REG, &return_code);
    if (0 != return_code) {
        fprintf(stderr, "%s: iom361_readReg gave return value %d\n", caller,
            return_code);
    }
    return return_code;
}

static int panel_write_leds(const char *caller, uint32_t *base,
    uint32_t leds) {
    int return_code;
    iom361_writeReg(base, LEDS_REG, leds, &return_code);
    if (0 != return_code) {
        fprintf(stderr, "%s: iom361_writeReg gave return value %d\n", caller,
            return_code);
    }
    return return_code;
}

int panel_led(uint32_t *base, int n, int on) {
    uint32_t leds;
    int return_code = panel_check_args("panel_led", base, n);
    if (0 != return_code) {
        return return_code;
    }
    return_code = panel_read_leds("panel_led", base, &leds);
    if (0 != return_code) {
        return return_code;
    }

    uint32_t mask = UINT32_C(1) << n;
    leds = on ? (leds | mask) : (leds & ~mask);
    return panel_write_leds("panel_led", base, leds);
}

int panel_toggle(uint32_t *base, int n) {
    uint32_t leds;
    int return_code = panel_check_args("panel_toggle", base, n);
    if (0 != return_code) {
        return return_code;
    }
    return_code = panel_read_leds("panel_toggle", base, &leds);
    if (0 != return_code) {
        return return_code;
    }

    leds ^= UINT32_C(1) << n;
    return panel_write_leds("panel_toggle", base, leds);
}
