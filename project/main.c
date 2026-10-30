#include "iom361_r4.h"
#include <stdio.h>

#define SWITCH_COUNT 3
#define LED_COUNT 2

int main(void) {
    int return_value;
    iom361_initialize(SWITCH_COUNT, LED_COUNT, &return_value);
    printf("Alarm Panel\n");
    if (0 != return_value)
        return 1;
    return 0;
}