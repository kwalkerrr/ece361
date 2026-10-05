#include "status.h"
#include "bits.h"
#include <stdio.h>

status_t status_unpack(uint16_t word) {
    status_t s;
    s.heat = get_field(word, STATUS_HEAT_POS, STATUS_HEAT_WIDTH);
    s.cool = get_field(word, STATUS_COOL_POS, STATUS_COOL_WIDTH);
    s.fan = get_field(word, STATUS_FAN_POS, STATUS_FAN_WIDTH);
    s.fault = get_field(word, STATUS_FAULT_POS, STATUS_FAULT_WIDTH);
    s.mode = get_field(word, STATUS_MODE_POS, STATUS_MODE_WIDTH);
    if (s.mode > STATUS_MAX_MODE) {
        fprintf(stderr, "status_unpack: word has invalid MODE field\n");
    }
    s.reserved = get_field(word, STATUS_RESERVED_POS, STATUS_RESERVED_WIDTH);
    if (s.reserved) {
        fprintf(stderr, "status_unpack: reserved bit of status word was not "
            "0\n");
    }
    s.setpoint = sign_extend(get_field(word, STATUS_SETPOINT_POS,
        STATUS_SETPOINT_WIDTH), STATUS_SETPOINT_WIDTH);
    return s;
}