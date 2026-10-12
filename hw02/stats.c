#include "stats.h"

static float max_from(const float data[], int i, int len) {
    if (i == len - 1)
        return data[i];
    float rest = max_from(data, i + 1, len);
    return data[i] > rest ? data[i] : rest;
}

float stats_max(const float data[], int len) {
    return max_from(data, 0, len);
}

float stats_min(const float data[], int len) {
    float min = data[0];
    for (int i = 1; i < len; i++) {
        if (data[i] < min) {
            min = data[i];
        }
    }
    return min;
}