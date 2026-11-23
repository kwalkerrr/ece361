#include "bst.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node *left;
    struct node *right;
} node_t;

struct bst {
    node_t *root;
    size_t size;
};

bst_t *bst_create(void) {
    bst_t *t = malloc(sizeof *t);
    if (t == NULL) {
        return NULL;
    }
    t->root = NULL;
    t->size = 0;
    return t;
}
