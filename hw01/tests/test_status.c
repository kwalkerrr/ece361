#include "../status.h"
#include <stdio.h>

static int failures = 0;

/* Returns true if every member of a matches the same member of b. */
static bool status_equal(status_t a, status_t b) {
    return a.heat == b.heat && a.cool == b.cool && a.fan == b.fan
        && a.fault == b.fault && a.mode == b.mode
        && a.reserved == b.reserved && a.setpoint == b.setpoint;
}

/* Prints every member of s on one line, prefixed by label. */
static void print_status(const char *label, status_t s) {
    printf("  %-9s heat=%d cool=%d fan=%d fault=%d mode=%u reserved=%d "
        "setpoint=%d\n", label, s.heat, s.cool, s.fan, s.fault,
        (unsigned)s.mode, s.reserved, s.setpoint);
}

/* Unpacks word, compares it against expected, and reports the result. */
static void check_status(const char *desc, uint16_t word, status_t expected) {
    status_t actual = status_unpack(word);
    bool pass = status_equal(expected, actual);
    printf("[%s] 0x%04X %s\n", pass ? "PASS" : "FAIL", (unsigned)word, desc);
    print_status("expected:", expected);
    print_status("actual:", actual);
    if (!pass) {
        failures++;
    }
}

static void test_status_unpack(void) {
    printf("\n=== status_unpack ===\n");

    // 0x31 = 0011 0001: heat on, mode 3 (AUTO); setpoint 0x16 = 22
    check_status("heat on, AUTO, 22 C", 0x1631, (status_t){
        .heat = true, .cool = false, .fan = false, .fault = false,
        .mode = 3, .reserved = false, .setpoint = 22});

    // 0x7A = 0111 1010: cool and fault on, mode 7 (invalid -> 0);
    // setpoint 0xF8 = -8
    check_status("invalid mode 7 (error)", 0xF87A, (status_t){
        .heat = false, .cool = true, .fan = false, .fault = true,
        .mode = 0, .reserved = false, .setpoint = -8});

    // 0xC5 = 1100 0101: heat and fan on, mode 4 (FAN ONLY), reserved set
    // (forced to false); setpoint 0xA5 = -91
    check_status("reserved bit set (error)", 0xA5C5, (status_t){
        .heat = true, .cool = false, .fan = true, .fault = false,
        .mode = 4, .reserved = false, .setpoint = -91});
}

int main(void) {
    // unbuffered so error messages on stderr line up with their test
    setvbuf(stdout, NULL, _IONBF, 0);

    test_status_unpack();

    printf("\n%d checked test(s) failed\n", failures);
    return failures == 0 ? 0 : 1;
}
