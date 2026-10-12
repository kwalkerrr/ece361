#include "reading.h"
#include <stdio.h>

reading_data_t reading_read(int ticks[], float temps[], float hums[], uint16_t
array_len, uint16_t line_len) {
    reading_data_t data;
    data.count = 0;
    data.skipped = 0;
    char line[line_len];

    while (fgets(line, sizeof line, stdin) != NULL) {
        int i = 0;
        while (line[i] == ' ' || line[i] == '\t')
            i++;
        if (line[i] == '\n' || line[i] == '\0' || line[i] == '#')
            continue;
        if (data.count == array_len) {
            fprintf(stderr, "warning: more than %d readings, the rest are "
                "ignored\n", array_len);
            break;
        }
        if (sscanf(line, "%d %f %f", &ticks[data.count], &temps[data.count],
        &hums[data.count]) == 3)
            data.count++;
        else
            data.skipped++;
    }

    return data;
}