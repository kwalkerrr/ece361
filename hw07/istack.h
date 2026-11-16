#ifndef HW_ISTACK_H
#define HW_ISTACK_H

#include <stddef.h>
#include <stdbool.h>

#define ISTACK_START 8 /* room at first; doubles when full */

typedef struct istack istack_t;

/**
 * Creates a new stack. The stack's data structure is a dynamic array with an
 * initial capacity of `ISTACK_START`. The stack should be freed after use with
 * `istack_destroy`.
 * @return `NULL` if there was not enough free memory space. Otherwise, a
 * pointer to the new stack.
 */
istack_t *istack_create(void);

/**
 * Deallocates all memory associated with the given stack.
 * @param s The stack to destroy.
 */
void istack_destroy(istack_t *s);

/**
 * Pushes `v` onto the stack. If the stack is full, the stack's array size is
 * doubled with `realloc`. Prints to `stderr` and does nothing if `s` is `NULL`
 * or if there was not enough free memory space.
 * @param s The stack to push to.
 * @param v The value to push.
 * @return -1 if `s` is `NULL` or if there was not enough free memory space.
 * Otherwise, 0.
 */
int istack_push(istack_t *s, int v);

/**
 * Puts the top value from the stack into `*v` (note that `v` may be `NULL`).
 * Additionally, this function removes the entry from the stack. Prints to
 * `stderr` and does nothing if `s` is `NULL` or if the stack is empty.
 * @param s The stack to pop from.
 * @param v The address of the variable in which to store the top value.
 * @return -1 if `s` is `NULL` or if the stack is empty. Otherwise, 0.
 */
int istack_pop(istack_t *s, int *v);

/**
 * Puts the top value from the stack into `*v` (note that `v` may be `NULL`).
 * Prints to `stderr` and does nothing if `s` is `NULL` or if the stack is
 * empty.
 * @param s The stack to peek into.
 * @param v The address of the variable in which to store the top value.
 * @return -1 if `s` is `NULL` or if the stack is empty. Otherwise, 0.
 */
int istack_peek(const istack_t *s, int *v);

/**
 * Gives the number of values on the stack. Prints to `stderr` if `s` is `NULL`.
 * @param s The stack to count the values of.
 * @return If `s` is `NULL`, 0. Otherwise, the number of values on the stack.
 */
size_t istack_count(const istack_t *s);

/**
 * Determines whether the stack is empty. Prints to `stderr` if `s` is `NULL`.
 * @param s The stack to check.
 * @return If `s` is `NULL`, `true`. Otherwise, `true` if the stack is empty and
 * `false` if it holds one or more values.
 */
bool istack_is_empty(const istack_t *s);

/**
 * Gives the total number of values that the stack is capable of holding at its
 * current array allocation size. Prints to `stderr` if `s` is `NULL`.
 * @param s The stack to determine the size of.
 * @return If `s` is `NULL`, 0. Otherwise, the current capacity of the stack.
 */
size_t istack_capacity(const istack_t *s);

/**
 * Copies the stack bottom-first to a new array. This new array is owned by the
 * caller; they must store its pointer and free it after use. Prints to `stderr`
 * if `s` is `NULL`, the stack is empty, there was not enough free memory
 * space to create a new array, or `n` is `NULL`.
 * @param s The stack to copy to a new array.
 * @param n The address of the variable in which to store the number of elements
 * in the new array. Must not be `NULL`. If `s` is `NULL`, the stack is empty,
 * or there was not enough free memory space to create a new array, `*n` will be
 * set to 0.
 * @return `NULL` if `s` is `NULL`, the stack is empty, there was not enough
 * free memory space to create a new array, or `n` is `NULL`. Otherwise, a
 * pointer to the new array.
 */
int *istack_to_array(const istack_t *s, size_t *n);

#endif
