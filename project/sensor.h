/*
This module handles all of the reading from the sensors. Each reading_t
measurement includes a timestamp (Unix-style), a temperature reading (degrees
Celsius), and a humidity reading (%RH). The latter two readings are rounded to
the nearest 0.1. This module does not print anything.
*/

#ifndef PROJ_SENSOR_H
#define PROJ_SENSOR_H

#include <stdint.h>

/**
 * Reads the temperature and humidity registers of iom361, converts them using
 * the relevant formulas, rounds them to the nearest 0.1, and then stores them
 * in *temp_celsius and *relative_humidity_percent. On a failed register read,
 * none of the input values are changed.
 * @param base The base address of the I/O register block.
 * @param temperature_celsius The register's converted temperature in degrees Celsius.
 * @param relative_humidity_percent The register's converted relative humidity
 * percentage.
 * @return 0 if the execution is successful. Otherwise, the return code of a
 * failed iom361_readReg call.
 */
int sensor_read(uint32_t *base, float *temp_celsius, float 
    *relative_humidity_percent);

#endif