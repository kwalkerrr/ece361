#include "ptr.h"

void swap_float(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}