#include "istack.h"

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

    // This a safe malloc with any reasonable SIZE_MAX.
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
