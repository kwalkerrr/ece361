#include "../tally.h"
#include <stdio.h>
#include <string.h>

#define PRINT_BUF 512 // room for everything tally_print writes in these tests

static int tests = 0;
static int failures = 0;

static void report(int pass) {
    tests++;
    if (!pass) {
        failures++;
    }
    printf("[%s] ", pass ? "PASS" : "FAIL");
}

static void check_int(const char *desc, int expected, int actual) {
    report(expected == actual);
    printf("%-50s expected %8d, actual %8d\n", desc, expected, actual);
}

static const char *null_str(const void *p) {
    return p == NULL ? "NULL" : "non-NULL";
}

static void check_null(const char *desc, int expect_null, const void *p) {
    report((p == NULL) == expect_null);
    printf("%-50s expected %8s, actual %8s\n", desc,
        expect_null ? "NULL" : "non-NULL", null_str(p));
}

static void check_true(const char *desc, int cond) {
    report(cond);
    printf("%-50s %s\n", desc, cond ? "yes" : "no");
}

// Fills buf with len copies of 'a', followed by '\0'.
static void make_name(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] = 'a';
    }
    buf[len] = '\0';
}

// Adds TALLY_NAMES different names ("name00", "name01", ...) to t.
static void fill(tally_t *t) {
    char name[TALLY_LEN + 1];
    for (int i = 0; i < TALLY_NAMES; i++) {
        snprintf(name, sizeof name, "name%02d", i);
        tally_add(t, name);
    }
}

// Runs tally_print on t and stores what it wrote in buf. Returns 0 if the
// output could not be captured.
static int capture_print(const tally_t *t, char *buf, size_t size) {
    buf[0] = '\0';
    FILE *f = tmpfile();
    if (f == NULL) {
        return 0;
    }
    tally_print(t, f);
    rewind(f);
    size_t n = fread(buf, 1, size - 1, f);
    buf[n] = '\0';
    fclose(f);
    return 1;
}

// Prints s in quotes, with each newline shown as \n.
static void print_escaped(const char *s) {
    putchar('"');
    for (; *s != '\0'; s++) {
        if (*s == '\n') {
            printf("\\n");
        } else {
            putchar(*s);
        }
    }
    putchar('"');
}

static void check_print(const char *desc, const tally_t *t,
    const char *expected) {
    char actual[PRINT_BUF];
    int captured = capture_print(t, actual, sizeof actual);

    report(captured && strcmp(expected, actual) == 0);
    printf("%-50s expected ", desc);
    print_escaped(expected);
    printf(", actual ");
    print_escaped(actual);
    printf("\n");
}

static void test_counting(void) {
    printf("\n=== test_counting ===\n");
    tally_t *t = tally_create();

    check_int("tally_size (new tally)", 0, tally_size(t));
    check_int("tally_add alice (1st time)", 1, tally_add(t, "alice"));
    check_int("tally_add alice (2nd time)", 2, tally_add(t, "alice"));
    check_int("tally_add alice (3rd time)", 3, tally_add(t, "alice"));
    check_int("tally_add bob (1st time)", 1, tally_add(t, "bob"));
    check_int("tally_count alice", 3, tally_count(t, "alice"));
    check_int("tally_count bob", 1, tally_count(t, "bob"));
    check_int("tally_count carol (never added)", 0, tally_count(t, "carol"));
    check_int("tally_size (repeats counted once)", 2, tally_size(t));
    tally_destroy(t);
}

static void test_name_lengths(void) {
    printf("\n=== test_name_lengths ===\n");
    tally_t *t = tally_create();
    char longest[TALLY_LEN + 1];
    char too_long[TALLY_LEN + 2];
    make_name(longest, TALLY_LEN);
    make_name(too_long, TALLY_LEN + 1);

    check_int("tally_add (empty name, error)", -1, tally_add(t, ""));
    check_int("tally_count (empty name)", 0, tally_count(t, ""));
    check_int("tally_size (after empty name)", 0, tally_size(t));
    check_int("tally_add (TALLY_LEN chars)", 1, tally_add(t, longest));
    check_int("tally_count (TALLY_LEN chars)", 1, tally_count(t, longest));
    check_int("tally_add (TALLY_LEN + 1 chars, error)", -1,
        tally_add(t, too_long));
    check_int("tally_count (TALLY_LEN + 1 chars)", 0,
        tally_count(t, too_long));
    check_int("tally_size (after TALLY_LEN + 1 chars)", 1, tally_size(t));
    tally_destroy(t);
}

static void test_null_args(void) {
    printf("\n=== test_null_args ===\n");
    tally_t *t = tally_create();
    tally_add(t, "alice");

    check_int("tally_add (NULL tally, error)", -1, tally_add(NULL, "alice"));
    check_int("tally_add (NULL name, error)", -1, tally_add(t, NULL));
    check_int("tally_count (NULL tally, error)", 0,
        tally_count(NULL, "alice"));
    check_int("tally_count (NULL name, error)", 0, tally_count(t, NULL));
    check_int("tally_size (NULL tally, error)", 0, tally_size(NULL));
    check_print("tally_print (NULL tally, error)", NULL, "");

    tally_print(t, NULL);
    report(1);
    printf("%-50s no crash\n", "tally_print (NULL stream, error)");
    tally_destroy(NULL);
    report(1);
    printf("%-50s no crash\n", "tally_destroy (NULL tally)");

    check_int("tally_count alice (unchanged by errors)", 1,
        tally_count(t, "alice"));
    tally_destroy(t);
}

static void test_full(void) {
    printf("\n=== test_full ===\n");
    tally_t *t = tally_create();
    fill(t);

    check_int("tally_size (full)", TALLY_NAMES, tally_size(t));
    check_int("tally_add new name (full, error)", -1, tally_add(t, "zed"));
    check_int("tally_count new name (full)", 0, tally_count(t, "zed"));
    check_int("tally_size (after new name refused)", TALLY_NAMES,
        tally_size(t));
    check_int("tally_add existing name (full)", 2, tally_add(t, "name00"));
    check_int("tally_count existing name (full)", 2,
        tally_count(t, "name00"));
    tally_destroy(t);
}

static void test_create_max(void) {
    printf("\n=== test_create_max ===\n");
    tally_t *t[TALLY_MAX];
    char label[64];

    for (int i = 0; i < TALLY_MAX; i++) {
        t[i] = tally_create();
        snprintf(label, sizeof label, "tally_create (tally %d of %d)", i + 1,
            TALLY_MAX);
        check_null(label, 0, t[i]);
    }
    int distinct = 1;
    for (int i = 0; i < TALLY_MAX; i++) {
        for (int j = i + 1; j < TALLY_MAX; j++) {
            distinct = distinct && t[i] != t[j];
        }
    }
    check_true("tally_create (all pointers different)", distinct);
    check_null("tally_create (TALLY_MAX + 1, error)", 1, tally_create());

    for (int i = 0; i < TALLY_MAX; i++) {
        tally_destroy(t[i]);
    }
}

static void test_destroy(void) {
    printf("\n=== test_destroy ===\n");
    tally_t *t[TALLY_MAX];
    for (int i = 0; i < TALLY_MAX; i++) {
        t[i] = tally_create();
    }
    tally_add(t[0], "alice");
    tally_destroy(t[0]);

    check_int("tally_add (destroyed, error)", -1, tally_add(t[0], "bob"));
    check_int("tally_count (destroyed, error)", 0,
        tally_count(t[0], "alice"));
    check_int("tally_size (destroyed, error)", 0, tally_size(t[0]));
    check_print("tally_print (destroyed, error)", t[0], "");

    tally_t *again = tally_create();
    check_null("tally_create (after destroying one)", 0, again);
    check_true("tally_create (reuses destroyed slot)", again == t[0]);
    check_int("tally_size (reused tally starts empty)", 0, tally_size(again));
    check_int("tally_count (old name gone)", 0, tally_count(again, "alice"));

    for (int i = 0; i < TALLY_MAX; i++) {
        tally_destroy(t[i]);
    }
}

static void test_print(void) {
    printf("\n=== test_print ===\n");
    tally_t *t = tally_create();

    check_print("tally_print (empty tally)", t, "");
    const char *names[] = {"dave", "alice", "carol", "alice", "bob", "dave",
        "alice"};
    for (size_t i = 0; i < sizeof names / sizeof names[0]; i++) {
        tally_add(t, names[i]);
    }
    check_print("tally_print (alphabetical order)", t,
        "alice 3\nbob 1\ncarol 1\ndave 2\n");
    tally_destroy(t);
}

int main(void) {
    test_counting();
    test_name_lengths();
    test_null_args();
    test_full();
    test_create_max();
    test_destroy();
    test_print();

    printf("\n%d of %d test(s) passed, %d failed\n", tests - failures, tests,
        failures);
    return failures == 0 ? 0 : 1;
}
