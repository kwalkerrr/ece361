#ifndef HW_STATS_H
#define HW_STATS_H

typedef struct {
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

/**
 * Finds the mean value of an array of floats.
 * @param data The array of floats to compute with.
 * @param len The number of elements in the array. Must be >= 1.
 * @return The mean value of data.
 */
float stats_mean(const float data[], int len);

/**
 * Finds the longest run of values above a threshold in an array of floats.
 * @param data The array of floats to search.
 * @param len The number of elements in the array.
 * @param threshold The value that an element must be greater than to start or
 * continue a streak.
 * @return A stats_run_t structure with the highest number of consecutive values
 * above threshold (best) and the first tick of that streak (best_start).
 * best_start will be -1 if best is 0 (i.e. there were no values > threshold).
 */
stats_run_t stats_threshold_run(const float data[], int len, float threshold);

#endif