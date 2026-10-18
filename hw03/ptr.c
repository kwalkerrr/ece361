#include "ptr.h"
#include <stdio.h>

void swap_float(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}

int minmax(const float *a, int n, float *min, float *max) {
    if (n < 1) {
        fprintf(stderr, "minmax: n of %d is < 1\n", n);
        return -1;
    }
    else if (a == NULL) {
        fprintf(stderr, "minmax: a is a NULL pointer\n");
        return -1;
    }
    else if (min == NULL) {
        fprintf(stderr, "minmax: min is a NULL pointer\n");
        return -1;
    }
    else if (max == NULL) {
        fprintf(stderr, "minmax: max is a NULL pointer\n");
        return -1;
    }

    *min = *a;
    *max = *a;
    for (const float *p = a + 1; p < a + n; p++) {
        if (*p < *min)
            *min = *p;
        else if (*p > *max)
            *max = *p;
    }

    return 0;
}

const float *find_above(const float *a, int n, float threshold) {
    if (n < 1) {
        fprintf(stderr, "find_above: n of %d is < 1\n", n);
        return NULL;
    }
    else if (a == NULL) {
        fprintf(stderr, "find_above: a is a NULL pointer\n");
        return NULL;
    }

    for (const float *p = a; p < a + n; p++) {
        if (*p > threshold)
            return p;
    }
    return NULL;
}