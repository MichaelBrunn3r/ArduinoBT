#ifndef BluetoothHC05_HEADER
#define BluetoothHC05_HEADER

#include "Arduino.h"
#include "SoftwareSerial.h"

#define BluetoothHC05_RESTART_TIMEOUT 5 // Timeout between turning the Module off and on again when restarting it
/** @brief Delay after turning the Module on and setting the Key pin to HIGH, when switching from powered off directly into Bluetooth::CMD_DATA_MODE.
  * Without or with to little of a delay, the module will enter Bluetooth::CMD_MODE instead. */
#define BluetoothHC05_CMD_DATA_MODE_KEY_DELAY 800 

class BluetoothHC05{
    public:
        const static bool NO_BAUD_RATE_FOUND = 0;

        enum Mode {
            /** @brief In this mode, the Moduel can be sent AT Commands */
            CMD_MODE, 
            /** @brief In this mode, the Module can transmit/receive data via Bluetooth */
            DATA_MODE, 
            /** @brief In this mode, the Module can transmit/receive data and can be sent AT Commands */
            CMD_DATA_MODE
        };

        /**
         * @brief Construct a new BluetoothHC05 object
         * 
         * @param pwrPIN The number of the Arduino pin which controls the power to the HC05 Module
         * @param keyPIN The number of the Arduino pin which is connected to the key pin of the HC05 Module 
         * @param txPIN The number of the Arduino pin which transmits data to the RX pin of the HC05 Module
         * @param rxPIN The number of the Arduino pin which receives data from the TX pin of the HC05 Module
         */
        BluetoothHC05(uint8_t pwrPIN, uint8_t keyPIN, uint8_t txPIN, uint8_t rxPIN);
        ~BluetoothHC05();

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
        void setTimeout(uint16_t timeout);
        size_t write(uint8_t byte);
        size_t write(const char* str);
        size_t write(const uint8_t* buffer, size_t size);
        size_t read();
        int readString();

        /**
         * @brief Changes the operation mode of the HC05 module.
         * 
         * This Method does not work asynchronously. Depending on its current mode and the new mode, this Method could block the Thread.
         * @param newMode The new mode of the HC05 module
         */
        void changeMode(Mode newMode);

        /**
         * @brief Determines and returns the Baudrate of the HC05 Module.
         * 
         * This Method tries to determine the Baudrate of the HC05 Module by communicating with it on known Baudrates.
         * This Method does not work asynchronous and will block the Thread. 
         * @param guess 
         * @return int The Baudrate of the HC05 Module, or NO_BAUD_RATE_FOUND, if no Baudrate was found
         */
        uint16_t determineBaud(uint16_t guess = 38400);

    private:
        uint8_t mPwrPIN; /** @brief The number of the arduino pin which controls the power to the HC05 Module */
        uint8_t mKeyPIN; /** @brief The number of the arduino pin which is connected to the key pin of the HC05 Module */
        SoftwareSerial* mSerial; /** @brief A pointer of the Serial connection to the HC05 Module */
        int mBaudRate;
        Mode mMode; /** The current mode of the HC05 Module */
        bool mIsOn = false;
};

#endif // BluetoothHC05_HEADER