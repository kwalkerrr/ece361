#include "tally.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 256 // longest line read at once, including the '\0'

// Throws away the rest of a line that did not fit in the buffer.
static void discard_rest(void) {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

// Reads one line from stdin into buf without its newline characters. A line
// too long for buf is cut short. Returns false at the end of input.
static bool read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        return false;
    }
    if (strchr(buf, '\n') == NULL && !feof(stdin)) {
        discard_rest();
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return true;
}

static bool is_skipped(const char *line) {
    return line[0] == '\0' || line[0] == '#';
}

int main(void) {
    tally_t *t = tally_create();
    if (t == NULL) {
        fprintf(stderr, "events: could not create a tally\n");
        return 1;
    }
    char line[BUF_SIZE];
    int refused = 0;
    while (read_line(line, BUF_SIZE)) {
        if (!is_skipped(line) && tally_add(t, line) == -1) {
            refused++;
        }
    }
    tally_print(t, stdout);
    printf("names: %d, lines skipped: %d\n", tally_size(t), refused);
    tally_destroy(t);
    return 0;
}
