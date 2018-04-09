#ifndef BluetoothHC05_HEADER
#define BluetoothHC05_HEADER

#include "Arduino.h"
#include "SoftwareSerial.h"

#define BTHC05_DEFAULT_SERIAL_TIMEOUT 100
// Timeout between turning the Module off and on again when restarting it
#define BTHC05_RESTART_TIMEOUT 5
/** @brief Delay after turning the Module on and setting the Key pin to HIGH, when switching from powered off directly into Bluetooth::CMD_DATA_MODE.
  * Without or with to little of a delay, the module will enter Bluetooth::CMD_MODE instead. */
#define BTHC05_CMD_DATA_MODE_KEY_DELAY 800
#define BTHC05_CMD_MODE_BAUDRATE 38400 
#define BTHC05_NO_BAUD_RATE_FOUND 0
#define BTHC05_NO_BAUD_RATE 0


class BluetoothHC05{
    public:
        /** @brief Enum which represents the different operation Modes of the HC05 Module */
        enum Mode {
            /** @brief In this mode, the Module can be sent AT Commands */
            CMD_MODE, 
            /** @brief In this mode, the Module can transmit/receive data via Bluetooth */
            DATA_MODE, 
            /** @brief In this mode, the Module can transmit/receive data and can be sent AT Commands */
            CMD_DATA_MODE
        };

        enum Role {
            SLAVE,
            MASTER
        };

        /**
         * @brief Construct a new BluetoothHC05 object
         * 
         * @param pwrPIN The number of the Arduino pin which controls the power to the HC05 Module
         * @param keyPIN The number of the Arduino pin which is connected to the key pin of the HC05 Module 
         * @param txPIN The number of the Arduino pin which transmits data to the RX pin of the HC05 Module
         * @param rxPIN The number of the Arduino pin which receives data from the TX pin of the HC05 Module
         */
        BluetoothHC05(uint8_t pwrPIN, uint8_t keyPIN, uint8_t txPIN, uint8_t rxPIN, uint32_t baudrate);
        ~BluetoothHC05();

        SoftwareSerial* serial();

        /**
         * @brief Returns true if the HC05 Module is powered.
         * 
         * This Method only returns a saved variable. To check the actual hardware, set \p checkHardware to true.
         * @param checkHardware If true, this Method will read the value of the Key pin instead
         * @return bool 
         */
        bool isOn(bool checkHardware = false);        
        void on();
        void off();
        int read();
        size_t readBytes(byte buffer[], size_t length);

        /**
         * @brief Changes the operation mode of the HC05 Module.
         * 
         * This Method does not work asynchronously. Depending on its current mode and the new mode, this Method could block the Thread.
         * @param newMode The new mode of the HC05 Module
         * @return Mode Returns the previous mode
         */
        Mode setMode(Mode newMode);

        /**
         * @brief Returns the current mode of the HC05 Module
         * 
         * @return Mode The current Mode of the HC05 Module
         */
        Mode getMode();

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
        bool setSerialParameters(uint32_t baudrate, bool stopBit=0, bool parityBit=0, bool checkReply=true);

        /**
         * @brief Determines and returns the Baudrate of the HC05 Module.
         * 
         * This Method tries to determine the Baudrate of the HC05 Module by communicating with it on known Baudrates.
         * This Method does not work asynchronous and will block the Thread. 
         * @param guess 
         * @return int The Baudrate of the HC05 Module, or NO_BAUD_RATE_FOUND, if no Baudrate was found
         */
        uint32_t determineBaud(uint32_t guess = 38400);

    private:
        uint8_t mPwrPIN; /** @brief The number of the arduino pin which controls the power to the HC05 Module */
        uint8_t mKeyPIN; /** @brief The number of the arduino pin which is connected to the key pin of the HC05 Module */
        SoftwareSerial* mSerial; /** @brief A pointer of the Serial connection to the HC05 Module */
        Mode mMode; /** @brief The current mode of the HC05 Module */
        bool mIsOn = false;

        /** @brief Struct containing the Serial Parameters of the HC05 Module */
        struct SerialParameters {
            uint32_t baudrate;
            bool stopBit;
            bool parityBit;
        } mSerialParameters;

        bool testBaudrate(uint16_t baudrate);

        
};

#endif // BluetoothHC05_HEADER