#include "tally.h"
#include <stdbool.h>
#include <stdio.h>

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
