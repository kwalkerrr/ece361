/* treestats.c: builds a binary search tree from the integers on stdin.
 * Usage: ./treestats < file
 * Inserts the integers in the order read, then prints one result line to
 * stdout. Exits with 0 on success, 1 if the input has something that is not a
 * number or memory runs out. */
#include "bst.h"

#include <stdio.h>
#include <stdlib.h>

/* How reading the input ended. */
typedef enum {
    READ_OK,         /* reached the end of the input */
    READ_NOT_NUMBER, /* found something that is not an integer */
    READ_NO_MEMORY   /* an insert failed */
} read_result_t;

/* Reads integers from stdin until the end of the input, inserting each into
 * `t` in the order read. */
static read_result_t read_keys(bst_t *t) {
    int key;
    int rc;

    while ((rc = scanf("%d", &key)) == 1) {
        if (bst_insert(t, key) != 0) {
            return READ_NO_MEMORY;
        }
    }
    return rc == EOF ? READ_OK : READ_NOT_NUMBER;
}

/* Prints the result line for `result`. Returns the exit status. */
static int report(const bst_t *t, read_result_t result) {
    switch (result) {
    case READ_NOT_NUMBER:
        printf("not a number after %zu values\n", bst_size(t));
        return EXIT_FAILURE;
    case READ_NO_MEMORY:
        fprintf(stderr, "out of memory after %zu values\n", bst_size(t));
        return EXIT_FAILURE;
    default:
        if (bst_size(t) == 0) {
            printf("empty tree\n");
        }
        else {
            printf("n %zu, height %zu\n", bst_size(t), bst_height(t));
        }
        return EXIT_SUCCESS;
    }
}

int main(void) {
    bst_t *t = bst_create();
    if (t == NULL) {
        fprintf(stderr, "out of memory\n");
        return EXIT_FAILURE;
    }

    int status = report(t, read_keys(t));
    bst_destroy(t);
    return status;
}
