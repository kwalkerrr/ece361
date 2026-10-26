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
