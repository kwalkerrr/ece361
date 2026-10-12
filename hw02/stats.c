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

float stats_mean(const float data[], int len) {
    float sum = 0.0f;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum / len;
}

stats_run_t stats_threshold_run(const float data[], int len, float threshold) {
    int run = 0, start = 0;
    stats_run_t best_info;
    best_info.best = 0;
    best_info.best_start = -1;
    for (int i = 0; i < len; i++) {
        if (data[i] > threshold) {
            if (run == 0)
                start = i;
            run++;
            if (run > best_info.best) {
                best_info.best = run;
                best_info.best_start = start;
            }
        } else {
            run = 0;
        }
    }
    return best_info;
}