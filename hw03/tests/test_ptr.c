#include "../ptr.h"
#include <stdio.h>

#define ARRAY_LEN 5

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

static void check_minmax(const char *desc, const float *a, int n,
    float exp_min, float exp_max) {
    char label[64];
    float min = 1.0f, max = 10.0f;
    minmax(a, n, &min, &max);

    snprintf(label, sizeof label, "minmax %s, min", desc);
    check_float(label, exp_min, min);
    snprintf(label, sizeof label, "minmax %s, max", desc);
    check_float(label, exp_max, max);
}

static void describe_ptr(char *buf, size_t size, const float *a, int n,
    const float *p) {
    if (p == NULL) {
        snprintf(buf, size, "NULL");
        return;
    }
    for (int i = 0; i < n; i++) {
        if (&a[i] == p) {
            snprintf(buf, size, "a[%d]", i);
            return;
        }
    }
    snprintf(buf, size, "outside a");
}

static void check_find_above(const char *desc, const float *a, int n,
    float threshold, int exp_index) {
    char label[64], exp_str[16], act_str[16];
    const float *expected = exp_index < 0 ? NULL : &a[exp_index];
    const float *actual = find_above(a, n, threshold);

    snprintf(label, sizeof label, "find_above %s", desc);
    describe_ptr(exp_str, sizeof exp_str, a, n, expected);
    describe_ptr(act_str, sizeof act_str, a, n, actual);
    report(expected == actual);
    printf("%-40s expected %9s, actual %9s\n", label, exp_str, act_str);
}

static void test_swap(void) {
    printf("\n=== test_swap ===\n");
    float x = 2.2f, y = 3.3f;
    
    swap_float(&x, &y);
    check_float("swap_float, x", 3.3f, x);
    check_float("swap_float, y", 2.2f, y);
}

static void test_minmax(void) {
    printf("\n=== test_minmax ===\n");

    float a1[ARRAY_LEN] = {1.3f, -7.1f, 8.4f, 10.3f, 3.5f};
    check_minmax("(min & max in middle)", a1, ARRAY_LEN, -7.1f, 10.3f);

    float a2[ARRAY_LEN] = {-7.1f, 1.3f, 8.4f, 10.3f, 3.5f};
    check_minmax("(min first)", a2, ARRAY_LEN, -7.1f, 10.3f);

    float a3[ARRAY_LEN] = {1.3f, 3.5f, 8.4f, 10.3f, -7.1f};
    check_minmax("(min last)", a3, ARRAY_LEN, -7.1f, 10.3f);

    float a4[ARRAY_LEN] = {10.3f, -7.1f, 8.4f, 1.3f, 3.5f};
    check_minmax("(max first)", a4, ARRAY_LEN, -7.1f, 10.3f);

    float a5[ARRAY_LEN] = {1.3f, -7.1f, 8.4f, 3.5f, 10.3f};
    check_minmax("(max last)", a5, ARRAY_LEN, -7.1f, 10.3f);

    float a6[ARRAY_LEN] = {8.1f, 8.1f, 8.1f, 8.1f, 8.1f};
    check_minmax("(repeated values)", a6, ARRAY_LEN, 8.1f, 8.1f);

    float a7[1] = {6.2f};
    check_minmax("(one value)", a7, 1, 6.2f, 6.2f);
    check_minmax("(n = 0, error)", a7, 0, 1.0f, 10.0f);

    float *a8 = NULL;
    check_minmax("(NULL pointer, error)", a8, ARRAY_LEN, 1.0f, 10.0f);
}

static void test_find_above(void) {
    printf("\n=== test_find_above ===\n");

    float a1[ARRAY_LEN] = {1.3f, 9.2f, 4.0f, 12.5f, 3.5f};
    check_find_above("(two matches)", a1, ARRAY_LEN, 5.0f, 1);

    float a2[ARRAY_LEN] = {1.3f, -7.1f, 4.0f, 2.2f, 3.5f};
    check_find_above("(no matches)", a2, ARRAY_LEN, 5.0f, -1);

    float a3[ARRAY_LEN] = {1.3f, 5.0f, 4.0f, 6.1f, 3.5f};
    check_find_above("(value equal to threshold)", a3, ARRAY_LEN, 5.0f, 3);
    check_find_above("(n = 0, error)", a3, 0, 5.0f, -1);

    float *a4 = NULL;
    check_find_above("(NULL pointer, error)", a4, ARRAY_LEN, 5.0f, -1);
}

int main(void) {
    test_swap();
    test_minmax();
    test_find_above();

    printf("\n%d of %d test(s) passed, %d failed\n", tests - failures, tests,
        failures);
    return failures == 0 ? 0 : 1;
}