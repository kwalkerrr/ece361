#ifndef HW_STATS_H
#define HW_STATS_H

typedef struct {
    int run;
    int start;
    int best;
    int best_start;
} stats_run_t;

/**
 * Recursively finds the maximum value of an array of floats.
 * @param data The array of floats to search.
 * @param len The number of elements in the array. Must be >= 1.
 * @return The highest value in data.
 */
float stats_max(const float data[], int len);

/**
 * Finds the minimum value of an array of floats.
 * @param data The array of floats to search.
 * @param len The number of elements in the array. Must be >= 1.
 * @return The lowest value in data.
 */
float stats_min(const float data[], int len);

#endif