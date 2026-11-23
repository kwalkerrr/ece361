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

/* Frees `n` and every node below it. Children are freed before their parent. */
static void node_destroy(node_t *n) {
    if (n == NULL) {
        return;
    }
    node_destroy(n->left);
    node_destroy(n->right);
    free(n);
}

void bst_destroy(bst_t *t) {
    if (t == NULL) {
        return;
    }
    node_destroy(t->root);
    free(t);
}
