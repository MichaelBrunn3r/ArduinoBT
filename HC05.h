#ifndef HC05_HEADER
#define HC05_HEADER

#include "Arduino.h"

// Macro that calculates the number of bits required to represent a number
    #define BITS_TO_REPRESENT_2(n) ((n&2)?1:0)
    #define BITS_TO_REPRESENT_4(n) ((n&(0xC))?(2+BITS_TO_REPRESENT_2(n>>2)):(BITS_TO_REPRESENT_2(n)))
    #define BITS_TO_REPRESENT_8(n) ((n&0xF0)?(4+BITS_TO_REPRESENT_4(n>>4)):(BITS_TO_REPRESENT_4(n)))
    #define BITS_TO_REPRESENT_16(n) ((n&0xFF00)?(8+BITS_TO_REPRESENT_8(n>>8)):(BITS_TO_REPRESENT_8(n)))
    #define BITS_TO_REPRESENT_32(n) ((n&0xFFFF0000)?(16+BITS_TO_REPRESENT_16(n>>16)):(BITS_TO_REPRESENT_16(n)))
    #define BITS_TO_REPRESENT(n) ((uint32_t)n==0?0:BITS_TO_REPRESENT_32((uint32_t)n)+1)

// Define the Modes in which a HC05 Module can be in
    /** @brief Enum which represents the different operation Modes of the HC05 Module */
    enum class HC05Mode : uint8_t {
        CMD_MODE, //< In this mode, the Module can be sent AT Commands
        DATA_MODE, //< In this mode, the Module can transmit/receive data via Bluetooth
        CMD_DATA_MODE, //< In this mode, the Module can transmit/receive data and can be sent AT Commands
        HC05Mode_MAX = 2
    };
    #define HC05_BITS_TO_REP_MODE BITS_TO_REPRESENT(HC05Mode::HC05Mode_MAX)

// Various timing Definitions for HC05 Module
    #define HC05_TIMINGS
    #define HC05_DEFAULT_SERIAL_TIMEOUT 100
    #define HC05_RESTART_TIMEOUT 5 // Timeout between turning the Module off and on again when restarting it
    #define HC05_RESTART_CMD_DATA_MODE_KEY_DELAY 850 /** Delay after turning the Module on and setting the Key pin to HIGH, when switching from powered off directly into HC05::CMD_DATA_MODE.
                                              *  Without or with to little of a delay, the module will enter HC05::CMD_MODE instead. */

// Definitions concerning Serial Parameters
    #define HC05_MAX_BAUDRATE 115200
    #define HC05_CMD_MODE_BAUDRATE 38400
    #define HC05_BITS_TO_REP_BAUD BITS_TO_REPRESENT(HC05_MAX_BAUDRATE)

    #define HC05_DEFAULT_STOP_BIT 0
    #define HC05_DEFAULT_PARITY_BIT 0

#endif // HC05_HEADER