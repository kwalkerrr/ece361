#include "iom361_r4.h"
#include "sensor.h"
#include <stdio.h>

#define SWITCH_COUNT 3
#define LED_COUNT 2

int main(void) {
    int return_value;
    uint32_t *base = iom361_initialize(SWITCH_COUNT, LED_COUNT, &return_value);
    printf("Alarm Panel\n");
    if (0 != return_value) {
        fprintf(stderr, "main: iom361 failed to initialize with code %d",
            return_value);
    }

    float temperature_celsius, relative_humidity_percent;
    sensor_read(base, &temperature_celsius, &relative_humidity_percent);
    printf("Temperature: %f degrees Celsius\n", temperature_celsius);
    printf("Relative Humidity: %f%% relative humidity\n",
        relative_humidity_percent);

    _iom361_set_sensor1(12.3f, 32.1f);
    sensor_read(base, &temperature_celsius, &relative_humidity_percent);
    printf("Temperature: %f degrees Celsius\n", temperature_celsius);
    printf("Relative Humidity: %f%% relative humidity\n",
        relative_humidity_percent);

    return 0;
}