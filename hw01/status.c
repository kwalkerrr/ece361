#include "status.h"
#include "bits.h"
#include <stdio.h>

#define STATUS_HEAT_POS 0
#define STATUS_HEAT_WIDTH 1
#define STATUS_COOL_POS 1
#define STATUS_COOL_WIDTH 1
#define STATUS_FAN_POS 2
#define STATUS_FAN_WIDTH 1
#define STATUS_FAULT_POS 3
#define STATUS_FAULT_WIDTH 1
#define STATUS_MODE_POS 4
#define STATUS_MODE_WIDTH 3
#define STATUS_RESERVED_POS 7
#define STATUS_RESERVED_WIDTH 1
#define STATUS_SETPOINT_POS 8
#define STATUS_SETPOINT_WIDTH 8
#define STATUS_MAX_MODE 4

status_t status_unpack(uint16_t word) {
    status_t s;
    s.heat = get_field(word, STATUS_HEAT_POS, STATUS_HEAT_WIDTH);
    s.cool = get_field(word, STATUS_COOL_POS, STATUS_COOL_WIDTH);
    s.fan = get_field(word, STATUS_FAN_POS, STATUS_FAN_WIDTH);
    s.fault = get_field(word, STATUS_FAULT_POS, STATUS_FAULT_WIDTH);
    s.mode = get_field(word, STATUS_MODE_POS, STATUS_MODE_WIDTH);
    if (s.mode > STATUS_MAX_MODE) {
        s.mode = 0;
        fprintf(stderr, "status_unpack: word has invalid MODE field; setting "
            "mode to 0 instead\n");
    }
    s.reserved = get_field(word, STATUS_RESERVED_POS, STATUS_RESERVED_WIDTH);
    if (s.reserved) {
        s.reserved = false;
        fprintf(stderr, "status_unpack: reserved bit of status word was not "
            "0; force-setting reserved to false anyway\n");
    }
    s.setpoint = sign_extend(get_field(word, STATUS_SETPOINT_POS,
        STATUS_SETPOINT_WIDTH), STATUS_SETPOINT_WIDTH);
    return s;
}