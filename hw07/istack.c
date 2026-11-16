#include "istack.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct istack {
    int *data;       /* dynamic array of values, bottom at index 0 */
    size_t count;    /* number of values currently on the stack */
    size_t capacity; /* number of values `data` can hold */
};

istack_t *istack_create(void) {
    istack_t *s = malloc(sizeof *s);
    if (s == NULL) {
        return NULL;
    }

    // This is a safe malloc with any reasonable SIZE_MAX.
    s->data = malloc(ISTACK_START * sizeof *s->data);
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->count = 0;
    s->capacity = ISTACK_START;
    return s;
}

void istack_destroy(istack_t *s) {
    if (s == NULL) {
        return;
    }

    free(s->data);
    free(s);
}

/* Doubles the capacity of `s->data`. Returns -1 (leaving `s` unchanged) if
 * the new size would overflow `size_t` or `realloc` fails. Otherwise, 0. */
static int istack_grow(istack_t *s) {
    if (s->capacity > SIZE_MAX / 2 / sizeof *s->data) {
        return -1;
    }

    size_t new_capacity = s->capacity * 2;
    int *new_data = realloc(s->data, new_capacity * sizeof *new_data);
    if (new_data == NULL) {
        return -1;
    }

    s->data = new_data;
    s->capacity = new_capacity;
    return 0;
}

int istack_push(istack_t *s, int v) {
    if (s == NULL) {
        fprintf(stderr, "istack_push: stack is NULL\n");
        return -1;
    }

    if (s->count == s->capacity && istack_grow(s) != 0) {
        fprintf(stderr, "istack_push: not enough memory to grow stack\n");
        return -1;
    }

    s->data[s->count] = v;
    s->count++;
    return 0;
}

/* Checks that `s` has a top value to read. Prints an error naming `caller` to
 * `stderr` and returns -1 if `s` is `NULL` or empty. Otherwise, 0. */
static int istack_check_top(const istack_t *s, const char *caller) {
    if (s == NULL) {
        fprintf(stderr, "%s: stack is NULL\n", caller);
        return -1;
    }

    if (s->count == 0) {
        fprintf(stderr, "%s: stack is empty\n", caller);
        return -1;
    }

    return 0;
}

int istack_pop(istack_t *s, int *v) {
    if (istack_check_top(s, "istack_pop") != 0) {
        return -1;
    }

    s->count--;
    if (v != NULL) {
        *v = s->data[s->count]; // after decrement above, points to last value
    }
    return 0;
}

int istack_peek(const istack_t *s, int *v) {
    if (istack_check_top(s, "istack_peek") != 0) {
        return -1;
    }

    if (v != NULL) {
        *v = s->data[s->count - 1];
    }
    return 0;
}

size_t istack_count(const istack_t *s) {
    if (s == NULL) {
        fprintf(stderr, "istack_count: stack is NULL\n");
        return 0;
    }

    return s->count;
}

bool istack_is_empty(const istack_t *s) {
    if (s == NULL) {
        fprintf(stderr, "istack_is_empty: stack is NULL\n");
        return true;
    }

    return s->count == 0;
}

size_t istack_capacity(const istack_t *s) {
    if (s == NULL) {
        fprintf(stderr, "istack_capacity: stack is NULL\n");
        return 0;
    }

    return s->capacity;
}

int *istack_to_array(const istack_t *s, size_t *n) {
    if (n == NULL) {
        fprintf(stderr, "istack_to_array: n is NULL\n");
        return NULL;
    }

    *n = 0;
    if (istack_check_top(s, "istack_to_array") != 0) {
        return NULL;
    }

    // count <= capacity, and capacity ints were already allocated, so this
    // multiplication cannot overflow.
    int *copy = malloc(s->count * sizeof *copy);
    if (copy == NULL) {
        fprintf(stderr, "istack_to_array: not enough memory for array\n");
        return NULL;
    }

    memcpy(copy, s->data, s->count * sizeof *copy);
    *n = s->count;
    return copy;
}
