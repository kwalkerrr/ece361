#ifndef HW_READING_H
#define HW_READING_H

typedef struct {
    int count;
    int skipped;
} reading_data_t;

#include <stdint.h>

/**
 * Reads data from stdin given in the format "<tick> <temperature> <humidity>".
 * Comments (text to the right of a '#') and blank lines are ignored. Lines that
 * don't hold three numbers are counted as skipped within the returned struct.
 * Once length valid values have been read, the rest of the lines are ignored
 * and an error is reported to stderr.
 * @param ticks Array for tick values to be stored in.
 * @param temps Array for temperature values to be stored in.
 * @param hums Array for humidity values to be stored in.
 * @param array_len Length of each array.
 * @param line_len Number of characters to read from each line.
 * @return A reading_data_t whose count member is the number of readings stored
 * and whose skipped member is the number of invalid lines.
 */
reading_data_t reading_read(int ticks[], float temps[], float hums[],
    uint16_t array_len, uint16_t line_len);

#endif