#include "../stats.h"
#include <stdio.h>

#define BIG_LEN 10000

static int tests = 0;
static int failures = 0;

/* Records one test result and prints PASS or FAIL for it. */
static void report(int pass) {
    tests++;
    if (!pass) {
        failures++;
    }
    printf("[%s] ", pass ? "PASS" : "FAIL");
}

/* Compares a float result against its expected value. Every expected value
 * used here is exactly representable as a float, so == is safe. */
static void check_f(const char *desc, float expected, float actual) {
    report(expected == actual);
    printf("%-40s expected %9.3f, actual %9.3f\n", desc, expected, actual);
}

/* Compares a stats_run_t result against its expected members. */
static void check_run(const char *desc, int best, int best_start,
    stats_run_t actual) {
    report(actual.best == best && actual.best_start == best_start);
    printf("%-40s expected %d from %d, actual %d from %d\n", desc, best,
        best_start, actual.best, actual.best_start);
}

static void test_one_value(void) {
    printf("\n=== one value ===\n");
    const float one[] = {7.5f};
    check_f("stats_min", 7.5f, stats_min(one, 1));
    check_f("stats_max", 7.5f, stats_max(one, 1));
    check_f("stats_mean", 7.5f, stats_mean(one, 1));
    check_run("stats_threshold_run, above", 1, 0,
        stats_threshold_run(one, 1, 5.0f));
    check_run("stats_threshold_run, below", 0, -1,
        stats_threshold_run(one, 1, 10.0f));
}

static void test_max_position(void) {
    printf("\n=== maximum at the start and at the end ===\n");
    const float start[] = {9.0f, 1.0f, 4.0f, 2.0f};
    const float end[] = {1.0f, 4.0f, 2.0f, 9.0f};
    check_f("stats_max, max at start", 9.0f, stats_max(start, 4));
    check_f("stats_max, max at end", 9.0f, stats_max(end, 4));
    check_f("stats_min, min at start", 1.0f, stats_min(end, 4));
    check_f("stats_min, min at end", 1.0f,
        stats_min((const float[]){4.0f, 2.0f, 9.0f, 1.0f}, 4));
    check_f("stats_max, all negative", -1.0f,
        stats_max((const float[]){-5.0f, -1.0f, -3.0f}, 3));
    check_f("stats_mean", 4.0f, stats_mean(start, 4));
}

static void test_threshold_run(void) {
    printf("\n=== stats_threshold_run ===\n");
    // a value equal to the threshold is not above it, so it ends the run
    const float equal[] = {31.0f, 30.0f, 31.0f, 31.0f};
    check_run("value == threshold breaks run", 2, 2,
        stats_threshold_run(equal, 4, 30.0f));
    check_run("all values == threshold", 0, -1,
        stats_threshold_run((const float[]){30.0f, 30.0f}, 2, 30.0f));
    check_run("tied runs, first one wins", 2, 0,
        stats_threshold_run((const float[]){35.0f, 35.0f, 0.0f, 35.0f, 35.0f},
            5, 30.0f));
    check_run("run reaches the end", 3, 2,
        stats_threshold_run((const float[]){35.0f, 0.0f, 35.0f, 35.0f, 35.0f},
            5, 30.0f));
}

static void test_big_max(void) {
    printf("\n=== %d values ===\n", BIG_LEN);
    // static so the 40 KB array is not on the stack
    static float big[BIG_LEN];
    for (int i = 0; i < BIG_LEN; i++) {
        big[i] = (float)(i % 1000);
    }
    check_f("stats_max, max repeated", 999.0f, stats_max(big, BIG_LEN));
    big[BIG_LEN - 1] = 5000.0f; // deepest level of the recursion
    check_f("stats_max, max at last index", 5000.0f,
        stats_max(big, BIG_LEN));
    big[0] = 6000.0f;
    check_f("stats_max, max at first index", 6000.0f,
        stats_max(big, BIG_LEN));
}

int main(void) {
    test_one_value();
    test_max_position();
    test_threshold_run();
    test_big_max();

    printf("\n%d of %d test(s) passed, %d failed\n", tests - failures, tests,
        failures);
    return failures == 0 ? 0 : 1;
}
