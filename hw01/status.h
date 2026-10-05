#ifndef HW_STATUS_H
#define HW_STATUS_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
    bool heat;
    bool cool;
    bool fan;
    bool fault;
    uint8_t mode;
    bool reserved;
    int8_t setpoint;
} status_t;

/**
 * Gives a status_t struct based on a 16-bit thermostat status word.
 * The following is a description of how the bits in the status word correspond
 * to the members in the status_t struct (format is "bit(s): member (desc)").
 *
 * - 0: heat (heater on)
 * 
 * - 1: cool (compressor on)
 * 
 * - 2: fan (fan on)
 * 
 * - 3: fault (fault detected)
 * 
 * - 4-6: mode (0 = OFF, 1 = HEAT, 2 = COOL, 3 = AUTO, 4 = FAN ONLY;
 *   5-7 invalid)
 * 
 * - 7: reserved (must be 0)
 * 
 * - 8-15: setpoint (set point in degrees Celsius; -128 to 127)
 *
 * @param word The 16-bit status word from the thermostat.
 * @return A status_t struct with corresponding variables for each part of the
 * status word.
 */
status_t status_unpack(uint16_t word);

#endif