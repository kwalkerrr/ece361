#ifndef HW_READING_H
#define HW_READING_H

typedef struct {
    int count;
    int skipped;
} reading_data_t;


/**
 * Reads data from stdin given in the format "<tick> <temperature> <humidity>".
 * Only reads up to length values. Comments (lines beginning with '#') and blank
 * lines are ignored. Lines that don't hold three numbers are counted as skipped
 * within the returned struct.
 * @param ticks[] Array for tick values to be stored in.
 * @param temps[] Array for temperature values to be stored in.
 * @param hums[] Array for humidity values to be stored in.
 * @param length Length of each array.
 * @return The "count" and "skipped" members based on how many lines contained
 * valid and invalid data respectively.
 */
reading_data_t reading_read(int ticks[], float temps[], float hums[],
    int length);

#endif