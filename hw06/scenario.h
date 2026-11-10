#ifndef HW_SCENARIO_H
#define HW_SCENARIO_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

/** The most data lines a scenario can hold. */
#define SCENARIO_MAX_LINES 1000

/** scenario_load could not open the file. */
#define SCENARIO_ERR_OPEN (-1)

/** The file had no `start` line or no data lines. */
#define SCENARIO_ERR_MISSING (-2)

/** One data line: the world as it is from `tick` until the next line. */
typedef struct {
    long tick;
    float temperature_celsius;
    float relative_humidity_percent;
    uint32_t switches;
} scenario_line_t;

/** A loaded scenario. Holds every data line in a fixed array, no malloc. */
typedef struct {
    time_t start;
    size_t count;
    scenario_line_t lines[SCENARIO_MAX_LINES];
} scenario_t;

/**
 * Loads a scenario file into `scenario`. The file format is:
 * 
 * - Text from `#` to the end of a line is a comment. Blank lines and lines that
 *   hold only a comment are skipped.
 * 
 * - The first line that is not skipped is `start <timestamp>`, where
 *   `<timestamp>` is a non-negative Unix timestamp in decimal.
 * 
 * - Every line after it is `<tick> <temp> <rh> <switches>`: a tick in seconds
 *   since the start (decimal), a temperature in degrees C, a relative humidity
 *   in %RH, and the switch register in hexadecimal (the `0x` is optional). The
 *   temperature and humidity must be finite; iom361 clamps out-of-range values.
 * 
 * - The first data line has tick 0, and every later tick is greater than the
 *   one before it.
 * 
 * - At most SCENARIO_MAX_LINES data lines.
 *
 * Line numbers count every line in the file, starting at 1, including comment
 * and blank lines. A line is wrong if it breaks a rule above: a bad or missing
 * field, an extra field, a value out of range, a data line before `start`, a
 * second `start`, a first tick that is not 0, a tick that does not increase, a
 * line longer than the internal buffer, or the data line after the first
 * SCENARIO_MAX_LINES.
 *
 * On any error, `scenario->count` is set to 0 and the rest of `*scenario` is
 * unspecified.
 * @param path The path of the scenario file.
 * @param scenario Where the scenario is stored. Must not be `NULL`.
 * @return 0 on success. The number of the first wrong line if there is one.
 * SCENARIO_ERR_OPEN (-1) if the file cannot be opened. SCENARIO_ERR_MISSING
 * (-2) if the file has no `start` line or no data lines.
 */
int scenario_load(const char *path, scenario_t *scenario);

/**
 * Finds the data line in force at `tick`: the last line whose tick is at or
 * before `tick`. Values hold between lines, so a tick between two lines gets
 * the earlier one, and a tick after the last line gets the last line.
 * @param scenario A scenario loaded by scenario_load. Must not be `NULL`.
 * @param tick Seconds since the start of the scenario.
 * @return The line in force, or `NULL` if `tick` is negative or the scenario
 * holds no lines.
 */
const scenario_line_t *scenario_at(const scenario_t *scenario, long tick);

#endif
