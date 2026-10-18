#include "../ptr.h"
#include <stdio.h>

static int tests = 0;
static int failures = 0;

static void report(int pass) {
    tests++;
    if (!pass) {
        failures++;
    }
    printf("[%s] ", pass ? "PASS" : "FAIL");
}

static void check_float(const char *desc, float expected, float actual) {
    report(expected == actual);
    printf("%-40s expected %9.3f, actual %9.3f\n", desc, expected, actual);
}

static void test_swap(void) {
    printf("\n=== swap_float ===\n");
    float x = 2.2f, y = 3.3f;
    swap_float(&x, &y);
    check_float("test_swap, x", 3.3f, x);
    check_float("test_swap, y", 2.2f, y);
}

int main(void) {
    test_swap();

    printf("\n%d of %d test(s) passed, %d failed\n", tests - failures, tests,
        failures);
    return failures == 0 ? 0 : 1;
}