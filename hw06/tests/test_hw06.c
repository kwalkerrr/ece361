#include "../iom361_r4.h"
#include "../panel.h"
#include "../scenario.h"
#include "../sensor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 160
#define SWITCH_COUNT 4
#define LED_COUNT 4
#define DATA_DIR "tests/data/"
#define START_TIME ((time_t) 1791273600)

static int passed = 0;
static int failed = 0;

/* Too big for the stack, like in main.c. */
static scenario_t scenario;

static void report(bool ok, const char *expected, const char *actual) {
    if (ok)
        passed++;
    else
        failed++;

    printf("%s\nexpected: %s\nactual:   %s\n\n",
           ok ? "[PASS]" : "[FAIL]", expected, actual);
}

/* ---- sensor ---- */

static void format_reading(char *buf, float temp, float rh, int rc,
                           const reading_t *r) {
    snprintf(buf, BUF_LEN, "sensor_read(%.1f, %.1f) -> rc %d, %.4f C, "
             "%.4f %%RH, time %lld", temp, rh, rc, r->temperature_celsius,
             r->relative_humidity_percent, (long long)r->timestamp);
}

static void test_sensor_read(uint32_t *base, float temp, float rh) {
    reading_t want = {START_TIME, temp, rh};
    reading_t got = {0, 0.0f, 0.0f};
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    _iom361_set_sensor1(temp, rh);
    int rc = sensor_read(base, START_TIME, &got);
    format_reading(want_str, temp, rh, 0, &want);
    format_reading(got_str, temp, rh, rc, &got);
    report(rc == 0 && got.timestamp == want.timestamp
           && got.temperature_celsius == want.temperature_celsius
           && got.relative_humidity_percent == want.relative_humidity_percent,
           want_str, got_str);
}

/* A wrong base address makes iom361_readReg fail, so the reading must keep
 * every value it had before. */
static void test_sensor_failed_read(void) {
    reading_t before = {12345, -1.5f, 2.5f};
    reading_t got = before;
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    int rc = sensor_read(NULL, START_TIME, &got);
    snprintf(want_str, BUF_LEN, "sensor_read(NULL) -> rc != 0, unchanged "
             "%lld, %.1f, %.1f", (long long)before.timestamp,
             before.temperature_celsius, before.relative_humidity_percent);
    snprintf(got_str, BUF_LEN, "sensor_read(NULL) -> rc %d, %lld, %.1f, %.1f",
             rc, (long long)got.timestamp, got.temperature_celsius,
             got.relative_humidity_percent);
    report(rc != 0 && got.timestamp == before.timestamp
           && got.temperature_celsius == before.temperature_celsius
           && got.relative_humidity_percent == before.relative_humidity_percent,
           want_str, got_str);
}

static void test_sensor(uint32_t *base) {
    test_sensor_read(base, 91.0f, 45.0f);
    test_sensor_failed_read();
}

/* ---- panel_pressed and panel_released ---- */

static void format_edges(char *buf, const char *what, uint32_t now,
                         uint32_t before, uint32_t pressed, uint32_t released) {
    snprintf(buf, BUF_LEN, "%s: now 0x%X, before 0x%X -> pressed 0x%X, "
             "released 0x%X", what, (unsigned)now, (unsigned)before,
             (unsigned)pressed, (unsigned)released);
}

static void test_edge(const char *what, uint32_t now, uint32_t before,
                      uint32_t want_pressed, uint32_t want_released) {
    uint32_t got_pressed = panel_pressed(now, before);
    uint32_t got_released = panel_released(now, before);
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    format_edges(want_str, what, now, before, want_pressed, want_released);
    format_edges(got_str, what, now, before, got_pressed, got_released);
    report(want_pressed == got_pressed && want_released == got_released,
           want_str, got_str);
}

static void test_edges(void) {
    test_edge("press", 0x1, 0x0, 0x1, 0x0);
    test_edge("release", 0x0, 0x1, 0x0, 0x1);
    test_edge("held", 0x1, 0x1, 0x0, 0x0);
    /* switch 0 held, 1 pressed, 2 released, 3 stays off */
    test_edge("mixed", 0x3, 0x5, 0x2, 0x4);
}

/* ---- panel_led and panel_toggle ---- */

static uint32_t read_leds(uint32_t *base) {
    return iom361_readReg(base, LEDS_REG, NULL);
}

static void set_leds(uint32_t *base, uint32_t value) {
    iom361_writeReg(base, LEDS_REG, value, NULL);
}

static void format_led(char *buf, const char *call, uint32_t start,
                       int rc, uint32_t leds) {
    snprintf(buf, BUF_LEN, "LEDs 0x%X, %s -> rc %d, LEDs 0x%X",
             (unsigned)start, call, rc, (unsigned)leds);
}

static void check_led(uint32_t *base, const char *call, uint32_t start,
                      int want_rc, uint32_t want_leds, int got_rc) {
    uint32_t got_leds = read_leds(base);
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    format_led(want_str, call, start, want_rc, want_leds);
    format_led(got_str, call, start, got_rc, got_leds);
    report(want_rc == got_rc && want_leds == got_leds, want_str, got_str);
}

static void test_led(uint32_t *base, uint32_t start, int n, int on,
                     int want_rc, uint32_t want_leds) {
    char call[BUF_LEN];

    set_leds(base, start);
    snprintf(call, BUF_LEN, "panel_led(%d, %d)", n, on);
    check_led(base, call, start, want_rc, want_leds, panel_led(base, n, on));
}

static void test_toggle(uint32_t *base, uint32_t start, int n,
                        int want_rc, uint32_t want_leds) {
    char call[BUF_LEN];

    set_leds(base, start);
    snprintf(call, BUF_LEN, "panel_toggle(%d)", n);
    check_led(base, call, start, want_rc, want_leds, panel_toggle(base, n));
}

static void test_leds(uint32_t *base) {
    test_led(base, 0xA, 0, 1, 0, 0xB);
    test_led(base, 0xA, 3, 0, 0, 0x2);
    test_led(base, 0xA, 1, 1, 0, 0xA);  /* already on */
    test_led(base, 0xA, 2, 0, 0, 0xA);  /* already off */
    test_led(base, 0xA, 32, 1, -1, 0xA); /* out of range: nothing changes */
    test_toggle(base, 0xA, 0, 0, 0xB);
    test_toggle(base, 0xA, 1, 0, 0x8);
    test_toggle(base, 0xA, -1, -1, 0xA); /* out of range: nothing changes */
}

/* ---- scenario ---- */

static void test_load(const char *file, int want) {
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    int got = scenario_load(file, &scenario);
    snprintf(want_str, BUF_LEN, "scenario_load(%s) -> %d", file, want);
    snprintf(got_str, BUF_LEN, "scenario_load(%s) -> %d", file, got);
    report(want == got, want_str, got_str);
}

static void format_line(char *buf, long tick, const scenario_line_t *line) {
    if (line == NULL)
        snprintf(buf, BUF_LEN, "scenario_at(%ld) -> NULL", tick);
    else
        snprintf(buf, BUF_LEN, "scenario_at(%ld) -> tick %ld, %.1f C, "
                 "%.1f %%RH, switches 0x%X", tick, line->tick,
                 line->temperature_celsius, line->relative_humidity_percent,
                 (unsigned)line->switches);
}

static bool same_line(const scenario_line_t *a, const scenario_line_t *b) {
    if (a == NULL || b == NULL)
        return a == b;
    return a->tick == b->tick && a->switches == b->switches
           && a->temperature_celsius == b->temperature_celsius
           && a->relative_humidity_percent == b->relative_humidity_percent;
}

static void test_at(long tick, const scenario_line_t *want) {
    const scenario_line_t *got = scenario_at(&scenario, tick);
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    format_line(want_str, tick, want);
    format_line(got_str, tick, got);
    report(same_line(want, got), want_str, got_str);
}

static void test_start(time_t want) {
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    snprintf(want_str, BUF_LEN, "start %lld, %d lines", (long long)want, 3);
    snprintf(got_str, BUF_LEN, "start %lld, %zu lines",
             (long long)scenario.start, scenario.count);
    report(want == scenario.start && scenario.count == 3, want_str, got_str);
}

/* comments.txt holds three data lines between comments and blank lines. */
static void test_comments(void) {
    const scenario_line_t line0 = {0, 21.5f, 40.0f, 0x0};
    const scenario_line_t line5 = {5, 91.0f, 45.5f, 0x1};
    const scenario_line_t line9 = {9, -10.0f, 99.9f, 0xF};

    test_load(DATA_DIR "comments.txt", 0);
    test_start(START_TIME);
    test_at(0, &line0);
    test_at(5, &line5);
    test_at(9, &line9);

    /* Values hold between lines and after the last one. */
    test_at(4, &line0);
    test_at(6, &line5);
    test_at(8, &line5);
    test_at(1000, &line9);
    test_at(-1, NULL);
}

static void test_bad_scenarios(void) {
    test_load(DATA_DIR "no_start.txt", 3);
    test_load(DATA_DIR "missing_field.txt", 3);
    test_load(DATA_DIR "same_tick.txt", 4);
    test_load(DATA_DIR "tick_goes_back.txt", 5);
    test_load(DATA_DIR "does_not_exist.txt", SCENARIO_ERR_OPEN);
}

int main(void) {
    int rc;
    uint32_t *base = iom361_initialize(SWITCH_COUNT, LED_COUNT, &rc);

    if (base == NULL) {
        fprintf(stderr, "iom361_initialize failed: %d\n", rc);
        return EXIT_FAILURE;
    }
    iom361_set_display(false);

    test_sensor(base);
    test_edges();
    test_leds(base);
    test_comments();
    test_bad_scenarios();

    printf("%d passed, %d failed\n", passed, failed);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
