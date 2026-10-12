#ifndef HW_HISTOGRAM_H
#define HW_HISTOGRAM_H

/**
 * Prints a histogram of an array of floats to stdout.
 * Bin b holds the values from b * bin_width up to (but not including)
 * (b + 1) * bin_width, so the bins cover 0 to num_bins * bin_width. Values
 * below 0 are counted in the first bin, and values at or above
 * num_bins * bin_width are counted in the last bin.
 * @param data The array of values to sort into bins.
 * @param len The number of elements in data. May be 0.
 * @param num_bins The number of bins. Must be >= 1.
 * @param bin_width The width of each bin. Must be > 0.
 */
void histogram_print(const float data[], int len, int num_bins,
    float bin_width);

#endif
