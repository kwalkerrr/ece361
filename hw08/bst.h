#ifndef HW_BST_H
#define HW_BST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct bst bst_t;

/**
 * Creates a new, empty tree. Must be freed with `bst_destroy`; the caller
 * should not free it themselves.
 * @return `NULL` if there was not enough memory available to create the tree.
 * Otherwise, a pointer to the new tree.
 */
bst_t *bst_create(void);

/**
 * Frees the tree and all of its nodes. If `t` is `NULL`, does nothing.
 * @param t The address of the tree to destroy.
 */
void bst_destroy(bst_t *t);

/**
 * Adds `key` to the tree. Smaller keys go left, all others go right. Duplicate
 * keys are kept. If `t` is `NULL` or there is not enough memory available, does
 * nothing. Ported from `insert` in BinarySearchTree.c.
 * @param t The address of the tree to add to.
 * @param key The value to add to the tree.
 * @return -1 if `t` is `NULL` or if there is not enough memory available to
 * insert `key`. Otherwise, 0.
 */
int bst_insert(bst_t *t, int key);

/**
 * Determines whether `key` is in the tree. If there are duplicate values, the
 * search stops at the first match. Ported from `search` in BinarySearchTree.c.
 * @param t The address of the tree to search.
 * @param key The value to check for.
 * @param visits The address of the variable in which to store the number of
 * nodes compared (whether the value is found or not). The address may be
 * `NULL`; if it is, the value is discarded. If `t` is `NULL` and `visits` is
 * not, `*visits` will be set to 0.
 * @return `false` if `t` is `NULL`. Otherwise, `true` if `key` is in the tree
 * and `false` if it is not in the tree.
 */
bool bst_contains(const bst_t *t, int key, size_t *visits);

/**
 * Determines the number of keys in the tree. Counts all keys, including
 * duplicates.
 * @param t The address of the tree to count the keys of.
 * @return If `t` is `NULL`, 0. Otherwise, the number of keys in the tree.
 */
size_t bst_size(const bst_t *t);

/**
 * Determines the number of nodes on the longest path from the root of the tree
 * down (i.e. the height of the tree). An empty tree gives 0, and a tree with
 * one node gives 1.
 * @param t The address of the tree to find the height of.
 * @return If `t` is `NULL`, 0. Otherwise, the height of the tree.
 */
size_t bst_height(const bst_t *t);

#endif
