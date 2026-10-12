#include "reading.h"
#include "stats.h"
#include "histogram.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_READINGS 1000
#define LINE_LEN     128
#define NUM_BINS     20          /* histogram: 0 to 100 C in 5 C bins */
#define BIN_WIDTH    5.0f

/* prints one "<label> min ... max ... mean ... <unit>" summary line */
static void print_stats(const char *label, const char *unit,
    const float data[], int len) {
    printf("%-12s min %6.1f  max %6.1f  mean %6.2f %s\n", label,
        stats_min(data, len), stats_max(data, len), stats_mean(data, len),
        unit);
}

/* prints the longest run of temperatures above threshold, by tick */
static void print_run(const int ticks[], const float temps[], int len,
    float threshold) {
    stats_run_t r = stats_threshold_run(temps, len, threshold);
    if (r.best == 0)
        printf("above %.1f C: never\n", threshold);
    else
        printf("above %.1f C: longest run %d readings, from tick %d to tick "
            "%d\n", threshold, r.best, ticks[r.best_start],
            ticks[r.best_start + r.best - 1]);
}

int main(int argc, char *argv[]) {
    float threshold = 30.0f;
    
    if (argc > 2) {
        fprintf(stderr, "usage: %s [threshold] < readings.txt\n", argv[0]);
        return 1;
    }
    if (argc == 2) {
        char *end;
        threshold = strtof(argv[1], &end);
        if (*end != '\0') {
            fprintf(stderr, "error: threshold '%s' is not a number\n", argv[1]);
            return 1;
        }
    }

    int   ticks[MAX_READINGS];
    float temps[MAX_READINGS];
    float hums[MAX_READINGS];
    reading_data_t data = reading_read(ticks, temps, hums, MAX_READINGS,
        LINE_LEN);
    
    printf("readings: %d\n", data.count);
    printf("skipped:  %d\n", data.skipped);
    if (data.count == 0) {
        printf("no readings, no summary\n");
        return 0;
    }

    print_stats("temperature:", "C", temps, data.count);
    print_stats("humidity:", "%RH", hums, data.count);
    print_run(ticks, temps, data.count, threshold);
    histogram_print(temps, data.count, NUM_BINS, BIN_WIDTH);
    return 0;
}