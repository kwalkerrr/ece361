#include "sensor.h"
#include "iom361_r4.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define DECIMAL_PLACES 1

static float sensor_round(float number) {
    float rounded_number;
    float shift = pow(10, DECIMAL_PLACES);
    rounded_number = number * shift;
    rounded_number = roundf(rounded_number);
    rounded_number /= shift;
    return rounded_number;
}

static float sensor_convert_temperature(uint32_t temperature_reading) {
    return temperature_reading / pow(2.0f, 20.0f) * 200.0f - 50.0f;
}

static float sensor_convert_humidity(uint32_t relative_humidity_reading) {
    return relative_humidity_reading / pow(2.0f, 20.0f) * 100.0f;
}

int sensor_read(uint32_t *base, float *temperature_celsius, float 
    *relative_humidity_percent) {
    int return_code;

    uint32_t temperature_reading = iom361_readReg(base, TEMP_REG, &return_code);
    if (0 != return_code) {
        fprintf(stderr, "sensor_read: iom361_readReg gave return value %d"
            " for temperature_reading", return_code);
        return return_code;
    }
    uint32_t relative_humidity_reading = iom361_readReg(base, HUMID_REG,
        &return_code);
    if (0 != return_code) {
        fprintf(stderr, "sensor_read: iom361_readReg gave return value %d"
            " for relative_humidity_reading", return_code);
        return return_code;
    }

    *temperature_celsius = sensor_convert_temperature(temperature_reading);
    *temperature_celsius = sensor_round(*temperature_celsius);

    *relative_humidity_percent =
        sensor_convert_humidity(relative_humidity_reading);
    *relative_humidity_percent = sensor_round(*relative_humidity_percent);\

    return return_code;
}