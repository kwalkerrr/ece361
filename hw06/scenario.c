#include "scenario.h"
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 256
#define START_FIELDS 2
#define DATA_FIELDS 4
#define MAX_FIELDS DATA_FIELDS
#define FIELD_SEPARATORS " \t\r\n"

static void strip_comment(char *line) {
    char *hash = strchr(line, '#');
    if (NULL != hash) {
        *hash = '\0';
    }
}

/* Splits `line` in place. Returns the number of fields, which can be more than
 * `max`; only the first `max` are stored. */
static int split_fields(char *line, char *fields[], int max) {
    int count = 0;
    for (char *field = strtok(line, FIELD_SEPARATORS); NULL != field;
        field = strtok(NULL, FIELD_SEPARATORS)) {
        if (count < max) {
            fields[count] = field;
        }
        count++;
    }
    return count;
}

static int parse_long(const char *text, long *value) {
    char *end;
    errno = 0;
    *value = strtol(text, &end, 10);
    return (0 == errno && end != text && '\0' == *end) ? 0 : -1;
}

static int parse_float(const char *text, float *value) {
    char *end;
    *value = strtof(text, &end);
    return (end != text && '\0' == *end && isfinite(*value)) ? 0 : -1;
}

static int parse_switches(const char *text, uint32_t *value) {
    char *end;
    if ('-' == text[0] || '+' == text[0]) {
        return -1;
    }
    errno = 0;
    unsigned long parsed = strtoul(text, &end, 16);
    if (0 != errno || end == text || '\0' != *end || parsed > UINT32_MAX) {
        return -1;
    }
    *value = (uint32_t) parsed;
    return 0;
}

static int parse_start(char *fields[], int count, time_t *start) {
    long value;
    if (START_FIELDS != count || 0 != strcmp(fields[0], "start")) {
        return -1;
    }
    if (0 != parse_long(fields[1], &value) || value < 0) {
        return -1;
    }
    *start = (time_t) value;
    return 0;
}

static int parse_data(char *fields[], int count, scenario_line_t *line) {
    if (DATA_FIELDS != count) {
        return -1;
    }
    if (0 != parse_long(fields[0], &line->tick)
        || 0 != parse_float(fields[1], &line->temperature_celsius)
        || 0 != parse_float(fields[2], &line->relative_humidity_percent)
        || 0 != parse_switches(fields[3], &line->switches)) {
        return -1;
    }
    return 0;
}

/* The first tick must be 0, and each tick after it must increase. */
static int check_tick(const scenario_t *scenario, long tick) {
    if (0 == scenario->count) {
        return (0 == tick) ? 0 : -1;
    }
    return (tick > scenario->lines[scenario->count - 1].tick) ? 0 : -1;
}

static int add_data(char *fields[], int count, scenario_t *scenario) {
    scenario_line_t line;
    if (SCENARIO_MAX_LINES == scenario->count) {
        return -1;
    }
    if (0 != parse_data(fields, count, &line)
        || 0 != check_tick(scenario, line.tick)) {
        return -1;
    }
    scenario->lines[scenario->count++] = line;
    return 0;
}

/* Returns 0 if `text` is a correct line, -1 if it is wrong. */
static int handle_line(char *text, scenario_t *scenario, int *have_start) {
    char *fields[MAX_FIELDS];
    strip_comment(text);
    int count = split_fields(text, fields, MAX_FIELDS);
    if (0 == count) {
        return 0;
    }
    if (!*have_start) {
        *have_start = 1;
        return parse_start(fields, count, &scenario->start);
    }
    return add_data(fields, count, scenario);
}

/* A line that filled the buffer without a newline before the end of the file
 * did not fit. */
static int line_too_long(const char *text, FILE *file) {
    return NULL == strchr(text, '\n') && !feof(file);
}

/* Returns 0, the number of the first wrong line, or SCENARIO_ERR_MISSING. */
static int read_lines(FILE *file, scenario_t *scenario) {
    char text[LINE_BUFFER_SIZE];
    int have_start = 0;
    int number = 0;
    while (NULL != fgets(text, sizeof text, file)) {
        number++;
        if (line_too_long(text, file)
            || 0 != handle_line(text, scenario, &have_start)) {
            return number;
        }
    }
    return (0 == scenario->count) ? SCENARIO_ERR_MISSING : 0;
}

int scenario_load(const char *path, scenario_t *scenario) {
    scenario->count = 0;
    FILE *file = fopen(path, "r");
    if (NULL == file) {
        return SCENARIO_ERR_OPEN;
    }
    int result = read_lines(file, scenario);
    fclose(file);
    if (0 != result) {
        scenario->count = 0;
    }
    return result;
}

const scenario_line_t *scenario_at(const scenario_t *scenario, long tick) {
    if (tick < 0 || 0 == scenario->count) {
        return NULL;
    }
    /* Binary search for the last line with lines[i].tick <= tick. lines[0]
     * has tick 0, so the answer is always in [low, high). */
    size_t low = 0;
    size_t high = scenario->count;
    while (high - low > 1) {
        size_t middle = low + (high - low) / 2;
        if (scenario->lines[middle].tick <= tick) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return &scenario->lines[low];
}
