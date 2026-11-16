#include "../istack.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 160
#define ARR_LEN 64 /* fits inside a BUF_LEN line with room for a prefix */
#define UNTOUCHED 42 /* value put in *v to detect unwanted writes */
#define MANY 1000

static int passed = 0;
static int failed = 0;

static void report(bool ok, const char *expected, const char *actual) {
    if (ok)
        passed++;
    else
        failed++;

    printf("%s\nexpected: %s\nactual:   %s\n\n",
           ok ? "[PASS]" : "[FAIL]", expected, actual);
}

/* Creates a stack, or exits: no test can run without one. */
static istack_t *new_stack(void) {
    istack_t *s = istack_create();
    if (s == NULL) {
        fprintf(stderr, "istack_create failed\n");
        exit(EXIT_FAILURE);
    }
    return s;
}

/* Pushes first, first + 1, ..., first + n - 1. Returns false if any push
 * fails. */
static bool push_range(istack_t *s, int first, int n) {
    for (int i = 0; i < n; i++) {
        if (istack_push(s, first + i) != 0)
            return false;
    }
    return true;
}

/* ---- pop and peek on an empty stack ---- */

static void test_empty_pop(void) {
    istack_t *s = new_stack();
    int v = UNTOUCHED;
    char got_str[BUF_LEN];

    int rc = istack_pop(s, &v);
    snprintf(got_str, BUF_LEN, "pop on empty -> rc %d, v %d", rc, v);
    report(rc == -1 && v == UNTOUCHED, "pop on empty -> rc -1, v 42", got_str);
    istack_destroy(s);
}

static void test_empty_peek(void) {
    istack_t *s = new_stack();
    int v = UNTOUCHED;
    char got_str[BUF_LEN];

    int rc = istack_peek(s, &v);
    snprintf(got_str, BUF_LEN, "peek on empty -> rc %d, v %d", rc, v);
    report(rc == -1 && v == UNTOUCHED, "peek on empty -> rc -1, v 42",
           got_str);
    istack_destroy(s);
}

/* ---- last in, first out ---- */

static void test_lifo(void) {
    istack_t *s = new_stack();
    int a = 0, b = 0, c = 0, top = 0;
    char got_str[BUF_LEN];

    push_range(s, 1, 3);
    istack_peek(s, &top);
    istack_pop(s, &a);
    istack_pop(s, &b);
    istack_pop(s, &c);
    snprintf(got_str, BUF_LEN, "push 1 2 3 -> peek %d, pops %d %d %d, "
             "empty %s", top, a, b, c, istack_is_empty(s) ? "yes" : "no");
    report(top == 3 && a == 3 && b == 2 && c == 1 && istack_is_empty(s),
           "push 1 2 3 -> peek 3, pops 3 2 1, empty yes", got_str);
    istack_destroy(s);
}

/* ---- count and capacity around the first growth ---- */

static void test_size(istack_t *s, size_t want_count, size_t want_cap) {
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];
    size_t count = istack_count(s);
    size_t cap = istack_capacity(s);

    snprintf(want_str, BUF_LEN, "count %zu, capacity %zu", want_count,
             want_cap);
    snprintf(got_str, BUF_LEN, "count %zu, capacity %zu", count, cap);
    report(count == want_count && cap == want_cap, want_str, got_str);
}

static void test_growth(void) {
    istack_t *s = new_stack();

    push_range(s, 0, ISTACK_START);
    test_size(s, ISTACK_START, ISTACK_START);
    push_range(s, ISTACK_START, 1);
    test_size(s, ISTACK_START + 1, 2 * ISTACK_START);
    istack_destroy(s);
}

/* ---- many pushes, then many pops ---- */

/* Pops everything and counts how many values came out in the order
 * MANY - 1, MANY - 2, ..., 0. */
static int pop_all_in_order(istack_t *s) {
    int correct = 0;
    for (int want = MANY - 1; want >= 0; want--) {
        int v;
        if (istack_pop(s, &v) == 0 && v == want)
            correct++;
    }
    return correct;
}

static void test_many(void) {
    istack_t *s = new_stack();
    char got_str[BUF_LEN];

    bool pushed = push_range(s, 0, MANY);
    snprintf(got_str, BUF_LEN, "1000 pushes -> all ok %s, count %zu",
             pushed ? "yes" : "no", istack_count(s));
    report(pushed && istack_count(s) == MANY,
           "1000 pushes -> all ok yes, count 1000", got_str);

    int correct = pop_all_in_order(s);
    snprintf(got_str, BUF_LEN, "1000 pops -> %d in order 999..0, count %zu",
             correct, istack_count(s));
    report(correct == MANY && istack_count(s) == 0,
           "1000 pops -> 1000 in order 999..0, count 0", got_str);
    istack_destroy(s);
}

/* ---- istack_to_array ---- */

/* Writes "[a, b, c]" into buf, which holds ARR_LEN chars. */
static void format_array(char *buf, const int *a, size_t n) {
    int len = snprintf(buf, ARR_LEN, "[");
    for (size_t i = 0; i < n && len < ARR_LEN; i++)
        len += snprintf(buf + len, ARR_LEN - len, i ? ", %d" : "%d", a[i]);
    if (len < ARR_LEN)
        snprintf(buf + len, ARR_LEN - len, "]");
}

static void test_to_array_order(void) {
    istack_t *s = new_stack();
    size_t n = 0;
    char arr_str[ARR_LEN] = "(null)";
    char got_str[BUF_LEN];

    push_range(s, 10, 3);
    int *a = istack_to_array(s, &n);
    if (a != NULL)
        format_array(arr_str, a, n);
    snprintf(got_str, BUF_LEN, "push 10 11 12 -> n %zu, %s", n, arr_str);
    report(a != NULL && n == 3 && a[0] == 10 && a[1] == 11 && a[2] == 12,
           "push 10 11 12 -> n 3, [10, 11, 12]", got_str);
    free(a);
    istack_destroy(s);
}

/* Changing the returned array must not change the stack. */
static void test_to_array_copy(void) {
    istack_t *s = new_stack();
    size_t n = 0;
    int top = 0;
    char got_str[BUF_LEN];

    push_range(s, 10, 3);
    int *a = istack_to_array(s, &n);
    if (a != NULL)
        a[n - 1] = -1;
    istack_peek(s, &top);
    snprintf(got_str, BUF_LEN, "set array top to -1 -> stack top %d, "
             "count %zu", top, istack_count(s));
    report(a != NULL && top == 12 && istack_count(s) == 3,
           "set array top to -1 -> stack top 12, count 3", got_str);
    free(a);
    istack_destroy(s);
}

static void test_to_array_empty(void) {
    istack_t *s = new_stack();
    size_t n = 99;
    char got_str[BUF_LEN];

    int *a = istack_to_array(s, &n);
    snprintf(got_str, BUF_LEN, "to_array on empty -> %s, n %zu",
             a == NULL ? "NULL" : "array", n);
    report(a == NULL && n == 0, "to_array on empty -> NULL, n 0", got_str);
    free(a);
    istack_destroy(s);
}

/* ---- NULL arguments ---- */

static void test_null_stack_ints(void) {
    int v = UNTOUCHED;
    char got_str[BUF_LEN];

    int push_rc = istack_push(NULL, 1);
    int pop_rc = istack_pop(NULL, &v);
    int peek_rc = istack_peek(NULL, &v);
    snprintf(got_str, BUF_LEN, "push %d, pop %d, peek %d, v %d", push_rc,
             pop_rc, peek_rc, v);
    report(push_rc == -1 && pop_rc == -1 && peek_rc == -1 && v == UNTOUCHED,
           "push -1, pop -1, peek -1, v 42", got_str);
}

static void test_null_stack_sizes(void) {
    char got_str[BUF_LEN];
    size_t count = istack_count(NULL);
    bool empty = istack_is_empty(NULL);
    size_t cap = istack_capacity(NULL);

    snprintf(got_str, BUF_LEN, "count %zu, is_empty %s, capacity %zu", count,
             empty ? "true" : "false", cap);
    report(count == 0 && empty && cap == 0,
           "count 0, is_empty true, capacity 0", got_str);
}

static void test_null_to_array(void) {
    istack_t *s = new_stack();
    size_t n = 99;
    char got_str[BUF_LEN];

    push_range(s, 1, 2);
    int *from_null = istack_to_array(NULL, &n);
    int *null_n = istack_to_array(s, NULL);
    snprintf(got_str, BUF_LEN, "to_array(NULL, &n) -> %s, n %zu; "
             "to_array(s, NULL) -> %s", from_null ? "array" : "NULL", n,
             null_n ? "array" : "NULL");
    report(from_null == NULL && n == 0 && null_n == NULL,
           "to_array(NULL, &n) -> NULL, n 0; to_array(s, NULL) -> NULL",
           got_str);
    free(from_null);
    free(null_n);
    istack_destroy(s);
}

/* v may be NULL: pop still removes the value, peek leaves it. */
static void test_null_v(void) {
    istack_t *s = new_stack();
    char got_str[BUF_LEN];

    push_range(s, 1, 2);
    int peek_rc = istack_peek(s, NULL);
    int pop_rc = istack_pop(s, NULL);
    snprintf(got_str, BUF_LEN, "peek(s, NULL) %d, pop(s, NULL) %d, "
             "count %zu", peek_rc, pop_rc, istack_count(s));
    report(peek_rc == 0 && pop_rc == 0 && istack_count(s) == 1,
           "peek(s, NULL) 0, pop(s, NULL) 0, count 1", got_str);
    istack_destroy(s);
}

static void test_null_destroy(void) {
    istack_destroy(NULL);
    report(true, "destroy(NULL) -> returns", "destroy(NULL) -> returns");
}

static void test_nulls(void) {
    test_null_stack_ints();
    test_null_stack_sizes();
    test_null_to_array();
    test_null_v();
    test_null_destroy();
}

/* ---- two stacks ---- */

static void test_independent(void) {
    istack_t *a = new_stack();
    istack_t *b = new_stack();
    int a_top = 0, b_top = 0;
    char got_str[BUF_LEN];

    push_range(a, 1, 20); /* grows a but not b */
    push_range(b, 100, 1);
    istack_pop(a, NULL);
    istack_peek(a, &a_top);
    istack_peek(b, &b_top);
    snprintf(got_str, BUF_LEN, "a: count %zu, top %d, capacity %zu; "
             "b: count %zu, top %d, capacity %zu", istack_count(a), a_top,
             istack_capacity(a), istack_count(b), b_top, istack_capacity(b));
    report(istack_count(a) == 19 && a_top == 19 && istack_capacity(a) == 32
           && istack_count(b) == 1 && b_top == 100
           && istack_capacity(b) == ISTACK_START,
           "a: count 19, top 19, capacity 32; b: count 1, top 100, "
           "capacity 8", got_str);
    istack_destroy(a);
    istack_destroy(b);
}

/* ---- destroying a non-empty stack ---- */

/* AddressSanitizer reports a leak at exit if destroy misses any memory. */
static void test_destroy_full(void) {
    istack_t *s = new_stack();
    push_range(s, 0, 50);
    istack_destroy(s);
    report(true, "destroy 50-value stack -> returns, no leak at exit",
           "destroy 50-value stack -> returns, no leak at exit");
}

int main(void) {
    test_empty_pop();
    test_empty_peek();
    test_lifo();
    test_growth();
    test_many();
    test_to_array_order();
    test_to_array_copy();
    test_to_array_empty();
    test_nulls();
    test_independent();
    test_destroy_full();

    printf("%d passed, %d failed\n", passed, failed);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
