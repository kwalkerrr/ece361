#include "iom361_r4.h"
#include "panel.h"
#include "scenario.h"
#include "sensor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define SWITCH_COUNT 4
#define LED_COUNT 4
#define REGISTER_BITS 32
#define TIME_BUFFER_SIZE 32
#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"

/* Kept static: a scenario_t holds 1000 lines, too big for the stack. */
static scenario_t scenario;

static void print_load_error(const char *path, int result) {
    if (SCENARIO_ERR_OPEN == result) {
        fprintf(stderr, "replay: cannot open %s\n", path);
    } else if (SCENARIO_ERR_MISSING == result) {
        fprintf(stderr, "replay: %s has no start line or no data lines\n",
            path);
    } else {
        fprintf(stderr, "replay: %s: line %d is wrong\n", path, result);
    }
}

static uint32_t *setup_iom361(void) {
    int return_code;
    uint32_t *base = iom361_initialize(SWITCH_COUNT, LED_COUNT, &return_code);
    if (NULL == base) {
        fprintf(stderr, "replay: iom361_initialize gave return value %d\n",
            return_code);
        return NULL;
    }
    iom361_set_display(false);
    return base;
}

static int read_register(uint32_t *base, uint32_t offset, uint32_t *value) {
    int return_code;
    *value = iom361_readReg(base, offset, &return_code);
    if (0 != return_code) {
        fprintf(stderr, "replay: iom361_readReg(0x%02X) gave return value %d\n",
            (unsigned) offset, return_code);
    }
    return return_code;
}

/* Formats `when` in UTC, so the trace is the same in every time zone. */
static int format_time(time_t when, char *buffer, size_t size) {
    struct tm *broken_down = gmtime(&when);
    if (NULL == broken_down || 0 == strftime(buffer, size, TIME_FORMAT,
        broken_down)) {
        fprintf(stderr, "replay: cannot format timestamp %lld\n",
            (long long) when);
        return -1;
    }
    return 0;
}

static int toggle_pressed(uint32_t *base, uint32_t pressed) {
    for (int k = 0; k < REGISTER_BITS; k++) {
        if (0 != (pressed & (UINT32_C(1) << k))) {
            int return_code = panel_toggle(base, k);
            if (0 != return_code) {
                return return_code;
            }
        }
    }
    return 0;
}

static void set_world(const scenario_line_t *line) {
    _iom361_set_sensor1(line->temperature_celsius,
        line->relative_humidity_percent);
    _iom361_set_switches(line->switches);
}

static int print_trace(time_t now, const reading_t *reading, uint32_t switches,
    uint32_t pressed, uint32_t leds) {
    char time_text[TIME_BUFFER_SIZE];
    if (0 != format_time(now, time_text, sizeof time_text)) {
        return -1;
    }
    printf("%s temp=%.1f rh=%.1f switches=0x%X pressed=0x%X leds=0x%X\n",
        time_text, reading->temperature_celsius,
        reading->relative_humidity_percent, (unsigned) switches,
        (unsigned) pressed, (unsigned) leds);
    return 0;
}

/* Runs one tick. `previous` holds the switch register from the tick before and
 * is updated to this tick's value. */
static int run_tick(uint32_t *base, long tick, uint32_t *previous) {
    reading_t reading;
    uint32_t switches, leds;
    time_t now = scenario.start + (time_t) tick;

    set_world(scenario_at(&scenario, tick));
    if (0 != sensor_read(base, now, &reading)
        || 0 != read_register(base, SWITCHES_REG, &switches)) {
        return -1;
    }
    uint32_t pressed = panel_pressed(switches, *previous);
    *previous = switches;
    if (0 != toggle_pressed(base, pressed)
        || 0 != read_register(base, LEDS_REG, &leds)) {
        return -1;
    }
    return print_trace(now, &reading, switches, pressed, leds);
}

static int replay(uint32_t *base) {
    /* iom361_initialize clears the switches, so tick 0 compares against 0. */
    uint32_t previous = 0;
    long last_tick = scenario.lines[scenario.count - 1].tick;
    for (long tick = 0; tick <= last_tick; tick++) {
        if (0 != run_tick(base, tick, &previous)) {
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "usage: %s scenario.txt\n", argv[0]);
        return 1;
    }
    int result = scenario_load(argv[1], &scenario);
    if (0 != result) {
        print_load_error(argv[1], result);
        return 1;
    }
    uint32_t *base = setup_iom361();
    if (NULL == base || 0 != replay(base)) {
        return 1;
    }
    return 0;
}
