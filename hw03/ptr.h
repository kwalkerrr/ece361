#ifndef HW_PTR_H
#define HW_PTR_H

/**
 * Swaps the value of two given floats.
 * @param a Address of first float.
 * @param b Address of second float.
 */
void swap_float(float *a, float *b);


/**
 * Stores the smallest and largest value of array a in *min and *max
 * respectively. Prints an error and returns if n is out of bounds or any
 * pointer is NULL.
 * @param a The array to read.
 * @param n The length of the array. Must be >= 1.
 * @param min Address in which to store minimum value.
 * @param max Address in which to store maximum value.
 * @return -1 if n < 1 or any pointer is NULL, 0 otherwise.
 */
int minmax(const float *a, int n, float *min, float *max);

/**
 * Finds the first value in array a above threshold, then returns a pointer to
 * that value. Prints an error and returns if n is out of bounds or a is a NULL
 * pointer.
 * @param a The array to read.
 * @param n The length of the array. Must be >= 1.
 * @param threshold The value against which array values will be compared.
 * @return A NULL pointer if there is no value of a above threshold or a is a
 * NULL pointer. Otherwise, a pointer to the first value of a above threshold.
 */
const float *find_above(const float *a, int n, float threshold);

/**
 * Reverses a[0...n-1] in place. Does nothing but throw an error if a is NULL or
 * n < 2.
 * @param a The array to reverse.
 * @param n The length of the array. Must be >= 2.
 */
void reverse(float *a, int n);

#endif