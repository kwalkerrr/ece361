#include "../bst.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 160
#define UNTOUCHED 42 /* value put in visits to detect missing writes */
#define SORTED_N 100

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

/* Creates a tree, or exits: no test can run without one. */
static bst_t *new_tree(void) {
    bst_t *t = bst_create();
    if (t == NULL) {
        fprintf(stderr, "bst_create failed\n");
        exit(EXIT_FAILURE);
    }
    return t;
}

/* Inserts keys[0], ..., keys[n - 1] in order. Returns false if any insert
 * fails. */
static bool insert_all(bst_t *t, const int *keys, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (bst_insert(t, keys[i]) != 0)
            return false;
    }
    return true;
}

/* The seven keys from BinarySearchTree.c. */
static const int SEVEN[] = { 27, 14, 35, 10, 19, 31, 42 };
#define SEVEN_N (sizeof SEVEN / sizeof SEVEN[0])

/* ---- the original's seven keys ---- */

static void test_seven_shape(void) {
    bst_t *t = new_tree();
    char got_str[BUF_LEN];

    bool ok = insert_all(t, SEVEN, SEVEN_N);
    snprintf(got_str, BUF_LEN, "insert 27 14 35 10 19 31 42 -> all ok %s, "
             "size %zu, height %zu", ok ? "yes" : "no", bst_size(t),
             bst_height(t));
    report(ok && bst_size(t) == 7 && bst_height(t) == 3,
           "insert 27 14 35 10 19 31 42 -> all ok yes, size 7, height 3",
           got_str);
    bst_destroy(t);
}

static void test_seven_search(void) {
    bst_t *t = new_tree();
    size_t found_visits = UNTOUCHED;
    size_t missing_visits = UNTOUCHED;
    char got_str[BUF_LEN];

    insert_all(t, SEVEN, SEVEN_N);
    bool found = bst_contains(t, 31, &found_visits);
    bool missing = bst_contains(t, 15, &missing_visits);
    snprintf(got_str, BUF_LEN, "contains 31 -> %s in %zu visits; "
             "contains 15 -> %s in %zu visits", found ? "true" : "false",
             found_visits, missing ? "true" : "false", missing_visits);
    report(found && found_visits == 3 && !missing && missing_visits == 3,
           "contains 31 -> true in 3 visits; contains 15 -> false in 3 visits",
           got_str);
    bst_destroy(t);
}

/* ---- duplicates ---- */

static void test_duplicate(void) {
    bst_t *t = new_tree();
    char got_str[BUF_LEN];

    int first = bst_insert(t, 5);
    int second = bst_insert(t, 5);
    snprintf(got_str, BUF_LEN, "insert 5 twice -> rc %d %d, size %zu",
             first, second, bst_size(t));
    report(first == 0 && second == 0 && bst_size(t) == 2,
           "insert 5 twice -> rc 0 0, size 2", got_str);
    bst_destroy(t);
}

/* ---- increasing order makes a straight line ---- */

static void test_sorted_height(void) {
    bst_t *t = new_tree();
    char want_str[BUF_LEN];
    char got_str[BUF_LEN];

    bool ok = true;
    for (int i = 0; i < SORTED_N && ok; i++)
        ok = bst_insert(t, i) == 0;
    snprintf(want_str, BUF_LEN, "insert 0..%d -> all ok yes, size %d, "
             "height %d", SORTED_N - 1, SORTED_N, SORTED_N);
    snprintf(got_str, BUF_LEN, "insert 0..%d -> all ok %s, size %zu, "
             "height %zu", SORTED_N - 1, ok ? "yes" : "no", bst_size(t),
             bst_height(t));
    report(ok && bst_size(t) == SORTED_N && bst_height(t) == SORTED_N,
           want_str, got_str);
    bst_destroy(t);
}

/* ---- NULL arguments ---- */

static void test_null_tree(void) {
    size_t visits = UNTOUCHED;
    char got_str[BUF_LEN];

    int rc = bst_insert(NULL, 1);
    bool found = bst_contains(NULL, 1, &visits);
    snprintf(got_str, BUF_LEN, "insert %d, contains %s, visits %zu, size %zu, "
             "height %zu", rc, found ? "true" : "false", visits,
             bst_size(NULL), bst_height(NULL));
    report(rc == -1 && !found && visits == 0 && bst_size(NULL) == 0
           && bst_height(NULL) == 0,
           "insert -1, contains false, visits 0, size 0, height 0", got_str);
}

static void test_null_visits(void) {
    bst_t *t = new_tree();
    char got_str[BUF_LEN];

    insert_all(t, SEVEN, SEVEN_N);
    bool found = bst_contains(t, 31, NULL);
    snprintf(got_str, BUF_LEN, "contains(t, 31, NULL) -> %s",
             found ? "true" : "false");
    report(found, "contains(t, 31, NULL) -> true", got_str);
    bst_destroy(t);
}

static void test_null_destroy(void) {
    bst_destroy(NULL);
    report(true, "destroy(NULL) -> returns", "destroy(NULL) -> returns");
}

/* ---- destroying a full tree ---- */

/* AddressSanitizer reports a leak at exit if destroy misses any node. */
static void test_destroy_full(void) {
    bst_t *t = new_tree();
    insert_all(t, SEVEN, SEVEN_N);
    insert_all(t, SEVEN, SEVEN_N); /* duplicates too */
    bst_destroy(t);
    report(true, "destroy 14-key tree -> returns, no leak at exit",
           "destroy 14-key tree -> returns, no leak at exit");
}

/* ---- the input that crashed: searching an empty tree ---- */

/* The original's search dereferenced the root without checking for NULL, so
 * searching an empty tree crashed. Kept to catch that crash if it returns;
 * stdout is flushed first so the earlier results survive a crash. */
static void test_empty_contains(void) {
    bst_t *t = new_tree();
    size_t visits = UNTOUCHED;
    char got_str[BUF_LEN];

    fflush(stdout);
    bool found = bst_contains(t, 5, &visits);
    snprintf(got_str, BUF_LEN, "contains 5 on empty -> %s, visits %zu",
             found ? "true" : "false", visits);
    report(!found && visits == 0, "contains 5 on empty -> false, visits 0",
           got_str);
    bst_destroy(t);
}

int main(void) {
    test_seven_shape();
    test_seven_search();
    test_duplicate();
    test_sorted_height();
    test_null_tree();
    test_null_visits();
    test_null_destroy();
    test_destroy_full();
    test_empty_contains();

    printf("%d passed, %d failed\n", passed, failed);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
