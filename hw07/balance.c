/* balance.c: checks that the brackets (), [], and {} on stdin are balanced.
 * Usage: ./balance < file
 * Prints one result line to stdout and exits with 0 if balanced, 1 if not. */
#include "istack.h"

#include <stdio.h>
#include <stdlib.h>

/* Everything the checker tracks while reading input. */
typedef struct {
    istack_t *stack; /* opening brackets not yet closed */
    size_t line;     /* 1-based line of the current character */
    size_t column;   /* 1-based column of the current character */
    size_t pairs;    /* number of matched pairs so far */
    size_t deepest;  /* largest number of brackets open at once */
} checker_t;

static bool is_open(int c) {
    return c == '(' || c == '[' || c == '{';
}

static bool is_close(int c) {
    return c == ')' || c == ']' || c == '}';
}

/* Returns the closing bracket that matches the opening bracket `open`. */
static int closer_for(int open) {
    switch (open) {
    case '(':
        return ')';
    case '[':
        return ']';
    default:
        return '}';
    }
}

/* Pushes `c` and updates the deepest nesting. Returns -1 on failure. */
static int handle_open(checker_t *ch, int c) {
    if (istack_push(ch->stack, c) != 0) {
        return -1;
    }

    size_t depth = istack_count(ch->stack);
    if (depth > ch->deepest) {
        ch->deepest = depth;
    }
    return 0;
}

/* Pops the innermost open bracket and checks that `c` closes it. Prints the
 * error and returns -1 if nothing is open or the brackets do not match. */
static int handle_close(checker_t *ch, int c) {
    if (istack_is_empty(ch->stack)) {
        printf("line %zu, column %zu: found '%c', nothing open\n",
               ch->line, ch->column, c);
        return -1;
    }

    int open;
    istack_pop(ch->stack, &open);
    if (closer_for(open) != c) {
        printf("line %zu, column %zu: found '%c', expected '%c'\n",
               ch->line, ch->column, c, closer_for(open));
        return -1;
    }

    ch->pairs++;
    return 0;
}

/* Processes one input character and advances the line/column position.
 * Returns -1 if the character makes the input unbalanced. */
static int handle_char(checker_t *ch, int c) {
    ch->column++;

    int result = 0;
    if (is_open(c)) {
        result = handle_open(ch, c);
    } else if (is_close(c)) {
        result = handle_close(ch, c);
    }

    if (c == '\n') {
        ch->line++;
        ch->column = 0;
    }
    return result;
}

/* Prints the result once all input has been read. Returns the exit status. */
static int report_end(const checker_t *ch) {
    if (!istack_is_empty(ch->stack)) {
        int innermost;
        istack_peek(ch->stack, &innermost);
        printf("end of input: %zu still open, innermost '%c'\n",
               istack_count(ch->stack), innermost);
        return EXIT_FAILURE;
    }

    printf("balanced: %zu pairs, deepest %zu\n", ch->pairs, ch->deepest);
    return EXIT_SUCCESS;
}

int main(void) {
    checker_t ch = {istack_create(), 1, 0, 0, 0};
    if (ch.stack == NULL) {
        fprintf(stderr, "balance: not enough memory for stack\n");
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    int c;
    while ((c = getchar()) != EOF) {
        if (handle_char(&ch, c) != 0) {
            status = EXIT_FAILURE;
            break;
        }
    }

    if (status == EXIT_SUCCESS) {
        status = report_end(&ch);
    }

    istack_destroy(ch.stack);
    return status;
}
