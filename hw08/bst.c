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

/*
 * Searches down from `current` for `key`, adding 1 to `*count` for each node
 * left behind on the way. Ported from `search` in BinarySearchTree.c.
 */
static bool node_search(const node_t *current, int key, size_t *count) {
    while (current->key != key) {
        (*count)++;

        if (current->key > key) {
            current = current->left;
        }
        else {
            current = current->right;
        }

        if (current == NULL) {
            return false;
        }
    }
    (*count)++; /* count the node that matched */
    return true;
}

bool bst_contains(const bst_t *t, int key, size_t *visits) {
    size_t count = 0;
    bool found = false;

    if (t != NULL) {
        found = node_search(t->root, key, &count);
    }
    if (visits != NULL) {
        *visits = count;
    }
    return found;
}

size_t bst_size(const bst_t *t) {
    if (t == NULL) {
        return 0;
    }
    return t->size;
}

/* Returns the number of nodes on the longest path down from `n`. */
static size_t node_height(const node_t *n) {
    if (n == NULL) {
        return 0;
    }
    size_t left = node_height(n->left);
    size_t right = node_height(n->right);
    return 1 + (left > right ? left : right);
}

size_t bst_height(const bst_t *t) {
    if (t == NULL) {
        return 0;
    }
    return node_height(t->root);
}
