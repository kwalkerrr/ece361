#include "bst.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct node {
    int key;
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

int bst_insert(bst_t *t, int key) {
    if (NULL == t) {
        return -1;
    }

    node_t *temp_node = malloc(sizeof *temp_node);
    if (NULL == temp_node) {
        return -1;
    }

    temp_node->key = key;
    temp_node->left = NULL;
    temp_node->right = NULL;

    if (NULL == t->root) {
        t->root = temp_node;
        t->size++;
        return 0;
    }
    else {
        node_t *current = t->root;
        node_t *parent = NULL;

        while (1) {
            parent = current;

            if (key < parent->key) {
                current = current->left;
                if (NULL == current) {
                    parent->left = temp_node;
                    t->size++;
                    return 0;
                }
            }
            else {
                current = current->right;
                if (NULL == current) {
                    parent->right = temp_node;
                    t->size++;
                    return 0;
                }
            }
        }
    }
}
