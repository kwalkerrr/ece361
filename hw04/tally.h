#ifndef HW_TALLY_H
#define HW_TALLY_H

#include <stdio.h>

#define TALLY_MAX 4 // tallies that can exist at the same time
#define TALLY_NAMES 16 // different names one tally can hold
#define TALLY_LEN 15 // longest name, without the '\0'

typedef struct tally tally_t;

/**
 * Creates a new, empty tally.
 * @return NULL if TALLY_MAX tallies already exist. Otherwise, a pointer to the
 * new tally.
 */
tally_t *tally_create(void);

/**
 * Frees up the specified tally so that tally_create can hand it out again. Note
 * that this does not deallocate any memory.
 * @param t A pointer to the tally to free up. If NULL, nothing happens.
 */
void tally_destroy(tally_t *t);

/**
 * Counts one more occurrence of name in the specified tally, then returns its
 * new count.
 * @param t A pointer to the tally to modify.
 * @param name The name to add an occurrence of.
 * @return -1 if t or name is NULL, name is empty or longer than TALLY_LEN, or
 * name is new and the tally is full. Otherwise, the name's new count in the
 * tally.
 */
int tally_add(tally_t *t, const char *name);


/**
 * Counts the occurrences of a name.
 * @param t A pointer to the tally to read from.
 * @param name The name to count instances of.
 * @return 0 if name is not in the tally or if t or name is NULL. Otherwise, the
 * number of occurrences of name in the tally.
 */
int tally_count(const tally_t *t, const char *name);

/**
 * Counts how many different names a tally holds.
 * @param t A pointer to the tally to read from.
 * @return 0 if t is NULL. Otherwise, the number of different names in the
 * tally.
 */
int tally_size(const tally_t *t);

/**
 * Prints out a line for each name in a tally in the format "<name> <count>"
 * alphabetically by name.
 * @param t A pointer to the tally to print out.
 * @param out The file stream to print to.
 */
void tally_print(const tally_t *t, FILE *out);

#endif