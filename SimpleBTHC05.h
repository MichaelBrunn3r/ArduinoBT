#ifndef SimpleBTHC05_HEADER
#define SimpleBTHC05_HEADER

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "HC05.h"

// Macro that calculates the number of bits required to represent a number
    #define BITS_TO_REPRESENT_2(n) ((n&2)?1:0)
    #define BITS_TO_REPRESENT_4(n) ((n&(0xC))?(2+BITS_TO_REPRESENT_2(n>>2)):(BITS_TO_REPRESENT_2(n)))
    #define BITS_TO_REPRESENT_8(n) ((n&0xF0)?(4+BITS_TO_REPRESENT_4(n>>4)):(BITS_TO_REPRESENT_4(n)))
    #define BITS_TO_REPRESENT_16(n) ((n&0xFF00)?(8+BITS_TO_REPRESENT_8(n>>8)):(BITS_TO_REPRESENT_8(n)))
    #define BITS_TO_REPRESENT_32(n) ((n&0xFFFF0000)?(16+BITS_TO_REPRESENT_16(n>>16)):(BITS_TO_REPRESENT_16(n)))
    #define BITS_TO_REPRESENT(n) ((uint32_t)n==0?0:BITS_TO_REPRESENT_32((uint32_t)n)+1)

#define HC05_BITS_TO_REP_PIN BITS_TO_REPRESENT(NUM_DIGITAL_PINS)

class SimpleBTHC05 {
    public:
        HardwareSerial* serial; /** @brief A pointer of the Serial connection to the HC05 Module */

        /**
         * @brief Construct a new SimpleBTHC05 object
         * 
         * @param pwrPIN The number of the Arduino pin which controls the power to the HC05 Module
         * @param keyPIN The number of the Arduino pin which is connected to the key pin of the HC05 Module 
         */
        SimpleBTHC05(HardwareSerial* serial, uint8_t pwrPIN, uint8_t keyPIN, uint32_t baudrate);
        ~SimpleBTHC05();

        void setup();

        /**
         * @brief Returns true if the HC05 Module is powered.
         * 
         * This Method only returns a saved variable. To check the actual hardware, set \p checkHardware to true.
         * @return bool 
         */
        bool isOn();
        /**
         * @brief Turns the HC05 Module on
         */
        void on();
        /**
         * @brief Turns the HC05 Module off
         */
        void off();
        /**
         * @brief Returns the current Mode of the HC05 Module 
         * 
         * @return uint8_t The current Mode of the HC05 Module
         */
        HC05Mode getMode();
        /**
         * @brief Set the Mode of the HC05 Module
         * 
         * @param newMode The new Mode of the HC05 Module
         * @return uint8_t The previous Mode of the HC05 Module
         */
        HC05Mode setMode(HC05Mode newMode);
        /**
         * @brief Returns the current Baudrate of the HC05 Module while in HC05Mode::DATA_MODE/HC05Mode::CMD_DATA_MODE
         * 
         * @return uint32_t The current Baudrate of the HC05 Module
         */
        uint32_t getBaudrate();
        /**
         * @brief Set the Serial Parameters (Baudrate, Stop Bit, Parity Bit) of the HC05 Module.
         * 
         * If \p checkReply is true, this Method waits for the reply of the HC05 Module and confirms if the execution was successfull.
         * This will take some time due to Serial Communication. 
         * @param baudrate The new baudrate
         * @param stopBit The new Stop Bit
         * @param parityBit The new Parity Bit
         * @param checkReply If true, confirms if the command was executed successfully (slower)
         * @return bool True, if execution was successfull
         */
        bool setSerialParameters(uint32_t baudrate, bool stopBit=HC05_DEFAULT_STOP_BIT, bool parityBit=HC05_DEFAULT_PARITY_BIT, bool checkReply=false);
        
    private:
        struct Container {
            uint8_t pwrPin : HC05_BITS_TO_REP_PIN; //< The number of the arduino pin which controls the power to the HC05 Module
            uint8_t keyPin : HC05_BITS_TO_REP_PIN; //< The number of the arduino pin which is connected to the key pin of the HC05 Module
            bool isOn:1;
            HC05Mode mode : HC05_BITS_TO_REP_MODE; //< The current Mode of operation of the HC05 Module
            uint32_t baudrate : HC05_BITS_TO_REP_BAUD; //< The current Baudrate of the HC05 Module while in HC05Mode::DATA_MODE/HC05Mode::CMD_DATA__MODE 
        } mContainer;
};

#endif // SimpleBTHC05_HEADER