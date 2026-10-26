#include "tally.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct entry {
    char name[TALLY_LEN + 1];
    int count;
};

struct tally {
    struct entry entries[TALLY_NAMES];
    int size; // entries in use, always the first `size` of them
};

static struct tally tallies[TALLY_MAX];
static bool used[TALLY_MAX]; // used[i] is true while tallies[i] is handed out

// True if t is a pool slot that is currently handed out.
static bool in_use(const tally_t *t) {
    for (int i = 0; i < TALLY_MAX; i++) {
        if (t == &tallies[i]) {
            return used[i];
        }
    }
    return false;
}

static bool valid_name(const char *name) {
    size_t len = strlen(name);
    return len > 0 && len <= TALLY_LEN;
}

// Returns the index of name in t, or the index where it would go to keep the
// entries sorted. Sets *found to whether name is already there.
static int find(const tally_t *t, const char *name, bool *found) {
    int i = 0;
    while (i < t->size && strcmp(t->entries[i].name, name) < 0) {
        i++;
    }
    *found = i < t->size && strcmp(t->entries[i].name, name) == 0;
    return i;
}

// Inserts name with a count of 0 at index i, shifting later entries up.
static void insert(tally_t *t, int i, const char *name) {
    // No need to worry about bounds as long as you check them before calling
    // this helper function.
    // Shifts up ith entry up to i+1 and so on, starting from the end so no
    // entry is overwritten before it has been copied.
    for (int j = t->size; j > i; j--) {
        t->entries[j] = t->entries[j - 1];
    }
    // valid_name should be called before this function to ensure safety of
    // strcpy.
    strcpy(t->entries[i].name, name);
    t->entries[i].count = 0;
    t->size++;
}

tally_t *tally_create(void) {
    for (int i = 0; i < TALLY_MAX; i++) {
        if (!used[i]) {
            used[i] = true;
            tallies[i].size = 0; // forget names left over from a destroyed tally
            return &tallies[i];
        }
    }
    return NULL;
}

void tally_destroy(tally_t *t) {
    for (int i = 0; i < TALLY_MAX; i++) {
        if (t == &tallies[i]) {
            used[i] = false;
            return;
        }
    }
}

int tally_add(tally_t *t, const char *name) {
    if (t == NULL || name == NULL) {
        fprintf(stderr, "tally_add: t and name must not be NULL\n");
        return -1;
    }
    if (!in_use(t)) {
        fprintf(stderr, "tally_add: tally is not in use\n");
        return -1;
    }
    if (!valid_name(name)) {
        fprintf(stderr, "tally_add: name must be 1 to %d characters\n", TALLY_LEN);
        return -1;
    }
    bool found;
    int i = find(t, name, &found);
    if (!found) {
        // Check if array is full to safely use insert helper function.
        if (t->size == TALLY_NAMES) {
            fprintf(stderr, "tally_add: tally is full\n");
            return -1;
        }
        insert(t, i, name);
    }
    return ++t->entries[i].count;
}

int tally_count(const tally_t *t, const char *name) {
    if (t == NULL || name == NULL) {
        fprintf(stderr, "tally_count: t and name must not be NULL\n");
        return 0;
    }
    if (!in_use(t)) {
        fprintf(stderr, "tally_count: tally is not in use\n");
        return 0;
    }
    bool found;
    int i = find(t, name, &found);
    return found ? t->entries[i].count : 0;
}
