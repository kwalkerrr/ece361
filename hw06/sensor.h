#ifndef HW_SENSOR_H
#define HW_SENSOR_H

#include <stdint.h>
#include <time.h>

typedef struct {
    time_t timestamp;
    float temperature_celsius;
    float relative_humidity_percent;
} reading_t;

/**
 * Reads the temperature and humidity registers of iom361, converts them using
 * the relevant formulas, rounds them to the nearest 0.1, and then stores them
 * in a `reading_t` struct in the `temperature_celsius` and
 * `relative_humidity_percent` fields. On a failed register read, none of the
 * input values are changed, and a message is printed to `stderr`.
 * @param base The base address of the I/O register block.
 * @param now The current timestamp.
 * @param reading The address of the `reading_t` struct in which to store the
 * temperature, relative humidity, and time values.
 * @return 0 if the execution is successful. Otherwise, the return code of a
 * failed iom361_readReg call.
 */
int sensor_read(uint32_t *base, time_t now, reading_t *reading);

#endif