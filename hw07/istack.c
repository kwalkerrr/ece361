#include "istack.h"

#include <stdint.h>
#include <stdio.h>

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
