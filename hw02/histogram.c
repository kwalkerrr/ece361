#include "histogram.h"
#include <stdio.h>

void histogram_print(const float data[], int len, int num_bins,
    float bin_width) {
    /* values outside 0..num_bins * bin_width go to the end bins */
    int bins[num_bins];
    for (int b = 0; b < num_bins; b++)
        bins[b] = 0;
    for (int i = 0; i < len; i++) {
        int b = (int) (data[i] / bin_width);
        if (data[i] < 0.0f)
            b = 0;
        if (b >= num_bins)
            b = num_bins - 1;
        bins[b]++;
    }
    printf("histogram:\n");
    for (int b = 0; b < num_bins; b++) {
        if (bins[b] == 0)
            continue;
        printf("  %5.1f to %5.1f C | ", b * bin_width, (b + 1) * bin_width);
        for (int k = 0; k < bins[b]; k++)
            putchar('*');
        printf(" %d\n", bins[b]);
    }
}
