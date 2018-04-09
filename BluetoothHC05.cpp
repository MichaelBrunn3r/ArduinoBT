#include "Arduino.h"
#include "BluetoothHC05.h"
#include "SoftwareSerial.h"
#include "StringUtils.h"

////////////////////////////
// Constructor/Destructor //
////////////////////////////

BluetoothHC05::BluetoothHC05(uint8_t pwrPIN, uint8_t keyPIN, uint8_t txPIN, uint8_t rxPIN, uint32_t baudrate) {
    // Configure Pins
    mPwrPIN = pwrPIN;
    mKeyPIN = keyPIN;
    pinMode(mPwrPIN, OUTPUT);
    pinMode(mKeyPIN, OUTPUT);

    // Configure Serial Connection
    mSerial = new SoftwareSerial(rxPIN,txPIN);
    mSerial->setTimeout(BTHC05_DEFAULT_SERIAL_TIMEOUT);
    mSerial->begin(BTHC05_CMD_MODE_BAUDRATE);

    // Set Serial Parameters
    mSerialParameters.baudrate = baudrate;
    mSerialParameters.stopBit = 0;
    mSerialParameters.parityBit = 0;

    mMode = CMD_MODE;
    digitalWrite(mKeyPIN, HIGH);
    off();
}

// BluetoothHC05::~BluetoothHC05();

////////////////////
// Public Methods //
////////////////////

SoftwareSerial* BluetoothHC05::serial() {
    return mSerial;
}

bool BluetoothHC05::isOn(bool checkHardware) {
    if(checkHardware) mIsOn = digitalRead(mPwrPIN);
    return mIsOn;
}

void BluetoothHC05::on() {
    if(mMode == CMD_DATA_MODE) digitalWrite(mKeyPIN, LOW);
    digitalWrite(mPwrPIN, HIGH);
    if(mMode == CMD_DATA_MODE) {
        delay(BTHC05_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mKeyPIN, LOW);
    }
    mIsOn = true;
}

void BluetoothHC05::off() {
    digitalWrite(mPwrPIN, LOW);
    mIsOn = false;
}

int BluetoothHC05::read() {
    return mSerial->read();
}

size_t BluetoothHC05::readBytes(byte buffer[], size_t length) {
    return mSerial->readBytes(buffer, length);
}

BluetoothHC05::Mode BluetoothHC05::setMode(Mode newMode) {
    // Don't do anything if already in the correct mode
    if(mMode == newMode) return;

    // When switching from or to BluetoothHC05::CMD_MODE, the HC05 Module has the be restarted.
    // First step is to turn off the module.
    if(mMode == CMD_MODE || newMode == CMD_MODE) off();

    // AT_COMMAND_MODE: Key = HIGH
    // COMMUNICATION_MODE: Key = LOW
    // AT_COMMUNICATION_MODE: Key = HIGH
    if(newMode == CMD_MODE) digitalWrite(mKeyPIN, HIGH);
    else digitalWrite(mKeyPIN, LOW); 

    // When switching to or from BluetoothHC05::CMD_MODE, restart HC05 Module after a short delay
    if(mMode == CMD_MODE || newMode == CMD_MODE) {
        delay(BTHC05_RESTART_TIMEOUT);
        on();
    }

    if(newMode == CMD_DATA_MODE) {
        if(mMode == CMD_MODE) delay(BTHC05_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mKeyPIN, HIGH);
    }

    // Switch to Baudrate of the new Mode.
    // CMD_MODE has a fixed Baudrate of BTHC05_CMD_MODE_BAUDRATE (typically 38400).
    // The Baudrate of the other modes is saved in BluetoothHC05::mSerialParameters.baudrate. */
    if(mMode == CMD_MODE) mSerial->begin(mSerialParameters.baudrate);
    else if(newMode == CMD_MODE) mSerial->begin(BTHC05_CMD_MODE_BAUDRATE);

    // Return old Mode
    Mode tmp = mMode;
    mMode = newMode;
    return tmp;
}

BluetoothHC05::Mode BluetoothHC05::getMode() {
    return mMode;
}   

bool BluetoothHC05::setSerialParameters(uint32_t baudrate, bool stopBit, bool parityBit, bool checkReply) {
    bool wasDataMode = (mMode == DATA_MODE);
    bool success = false;

    char* atCmd = new char[24]{};
    sprintf(atCmd, "AT+UART=%u,%u,%u\r\n", baudrate, stopBit, parityBit);

    // Send Command
    if(mMode == DATA_MODE) {
        // Can't send commands in DATA_MODE, first switch to CMD_DATA_MODE
        setMode(CMD_DATA_MODE);
        mSerial->flush();
        mSerial->write(atCmd);
    } else mSerial->write(atCmd);

    // If specified: Check reply to confirm if command was successfull
    if(checkReply && mSerial->available()) {
        size_t replyLen = 128;
        char* reply = new char[replyLen]{};
        readBytes(reply, replyLen);
        success = StringUtils::contains(reply, "OK") ;

        Serial.print("Reply was: ");
        Serial.println(reply);
    } else success = true; // Else, assume successfull execution

    // If Module was in DATA_MODE, switch back to it
    if(wasDataMode) setMode(DATA_MODE);

    // If execution was successfull, save new Serial Parameters
    if(success) {
        mSerialParameters.baudrate = baudrate;
        mSerialParameters.stopBit = stopBit;
        mSerialParameters.parityBit = parityBit;

        // Baudrate of Module changed -> Change SoftwareSerial Baudrate
        if(mMode != CMD_MODE) mSerial->begin(mSerialParameters.baudrate);
    }

    return success;
}

uint32_t BluetoothHC05::determineBaud(uint32_t guess) {
    digitalWrite(mKeyPIN, HIGH);

    // The rates that will be tested
    uint32_t rates[] = {guess,38400,9600,115200,75880,57600,19200,4800,2400,1200,300};
    size_t num_rates = sizeof(rates)/sizeof(uint16_t);

    // Test the Baud rates
    for(int i=0; i<num_rates; i++) {
        uint16_t rate = rates[i];
        testBaudrate(rate);
    }

    return BTHC05_NO_BAUD_RATE_FOUND;
}

/////////////////////
// Private Methods //
/////////////////////

bool BluetoothHC05::testBaudrate(uint16_t baudrate) {
    size_t buf_size = 128;
    char* buf = new char[buf_size]{};

    mSerial->begin(baudrate);
    mSerial->flush();
    mSerial->write("AT\r\n");
    return mSerial->readBytes(buf, buf_size) > 0;
}#include "Arduino.h"
#include "BluetoothHC05.h"
#include "SoftwareSerial.h"
#include "StringUtils.h"

////////////////////////////
// Constructor/Destructor //
////////////////////////////

BluetoothHC05::BluetoothHC05(uint8_t pwrPIN, uint8_t keyPIN, uint8_t txPIN, uint8_t rxPIN, uint32_t baudrate) {
    // Configure Pins
    mPwrPIN = pwrPIN;
    mKeyPIN = keyPIN;
    pinMode(mPwrPIN, OUTPUT);
    pinMode(mKeyPIN, OUTPUT);

    // Configure Serial Connection
    mSerial = new SoftwareSerial(rxPIN,txPIN);
    mSerial->setTimeout(BTHC05_DEFAULT_SERIAL_TIMEOUT);
    mSerial->begin(BTHC05_CMD_MODE_BAUDRATE);

    // Set Serial Parameters
    mSerialParameters.baudrate = baudrate;
    mSerialParameters.stopBit = 0;
    mSerialParameters.parityBit = 0;

    mMode = CMD_MODE;
    digitalWrite(mKeyPIN, HIGH);
    off();
}

// BluetoothHC05::~BluetoothHC05();

////////////////////
// Public Methods //
////////////////////

SoftwareSerial* BluetoothHC05::serial() {
    return mSerial;
}

bool BluetoothHC05::isOn(bool checkHardware) {
    if(checkHardware) mIsOn = digitalRead(mPwrPIN);
    return mIsOn;
}

void BluetoothHC05::on() {
    if(mMode == CMD_DATA_MODE) digitalWrite(mKeyPIN, LOW);
    digitalWrite(mPwrPIN, HIGH);
    if(mMode == CMD_DATA_MODE) {
        delay(BTHC05_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mKeyPIN, LOW);
    }
    mIsOn = true;
}

void BluetoothHC05::off() {
    digitalWrite(mPwrPIN, LOW);
    mIsOn = false;
}

int BluetoothHC05::read() {
    return mSerial->read();
}

size_t BluetoothHC05::readBytes(byte buffer[], size_t length) {
    return mSerial->readBytes(buffer, length);
}

BluetoothHC05::Mode BluetoothHC05::setMode(Mode newMode) {
    // Don't do anything if already in the correct mode
    if(mMode == newMode) return;

    // When switching from or to BluetoothHC05::CMD_MODE, the HC05 Module has the be restarted.
    // First step is to turn off the module.
    if(mMode == CMD_MODE || newMode == CMD_MODE) off();

    // AT_COMMAND_MODE: Key = HIGH
    // COMMUNICATION_MODE: Key = LOW
    // AT_COMMUNICATION_MODE: Key = HIGH
    if(newMode == CMD_MODE) digitalWrite(mKeyPIN, HIGH);
    else digitalWrite(mKeyPIN, LOW); 

    // When switching to or from BluetoothHC05::CMD_MODE, restart HC05 Module after a short delay
    if(mMode == CMD_MODE || newMode == CMD_MODE) {
        delay(BTHC05_RESTART_TIMEOUT);
        on();
    }

    if(newMode == CMD_DATA_MODE) {
        if(mMode == CMD_MODE) delay(BTHC05_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mKeyPIN, HIGH);
    }

    // Switch to Baudrate of the new Mode.
    // CMD_MODE has a fixed Baudrate of BTHC05_CMD_MODE_BAUDRATE (typically 38400).
    // The Baudrate of the other modes is saved in BluetoothHC05::mSerialParameters.baudrate. */
    if(mMode == CMD_MODE) mSerial->begin(mSerialParameters.baudrate);
    else if(newMode == CMD_MODE) mSerial->begin(BTHC05_CMD_MODE_BAUDRATE);

    // Return old Mode
    Mode tmp = mMode;
    mMode = newMode;
    return tmp;
}

BluetoothHC05::Mode BluetoothHC05::getMode() {
    return mMode;
}   

bool BluetoothHC05::setSerialParameters(uint32_t baudrate, bool stopBit, bool parityBit, bool checkReply) {
    bool wasDataMode = (mMode == DATA_MODE);
    bool success = false;

    char* atCmd = new char[24]{};
    sprintf(atCmd, "AT+UART=%u,%u,%u\r\n", baudrate, stopBit, parityBit);

    // Send Command
    if(mMode == DATA_MODE) {
        // Can't send commands in DATA_MODE, first switch to CMD_DATA_MODE
        setMode(CMD_DATA_MODE);
        mSerial->flush();
        mSerial->write(atCmd);
    } else mSerial->write(atCmd);

    // If specified: Check reply to confirm if command was successfull
    if(checkReply && mSerial->available()) {
        size_t replyLen = 128;
        char* reply = new char[replyLen]{};
        readBytes(reply, replyLen);
        success = StringUtils::contains(reply, "OK") ;

        Serial.print("Reply was: ");
        Serial.println(reply);
    } else success = true; // Else, assume successfull execution

    // If Module was in DATA_MODE, switch back to it
    if(wasDataMode) setMode(DATA_MODE);

    // If execution was successfull, save new Serial Parameters
    if(success) {
        mSerialParameters.baudrate = baudrate;
        mSerialParameters.stopBit = stopBit;
        mSerialParameters.parityBit = parityBit;

        // Baudrate of Module changed -> Change SoftwareSerial Baudrate
        if(mMode != CMD_MODE) mSerial->begin(mSerialParameters.baudrate);
    }

    return success;
}

uint32_t BluetoothHC05::determineBaud(uint32_t guess) {
    digitalWrite(mKeyPIN, HIGH);

    // The rates that will be tested
    uint32_t rates[] = {guess,38400,9600,115200,75880,57600,19200,4800,2400,1200,300};
    size_t num_rates = sizeof(rates)/sizeof(uint16_t);

    // Test the Baud rates
    for(int i=0; i<num_rates; i++) {
        uint16_t rate = rates[i];
        testBaudrate(rate);
    }

    return BTHC05_NO_BAUD_RATE_FOUND;
}

/////////////////////
// Private Methods //
/////////////////////

bool BluetoothHC05::testBaudrate(uint16_t baudrate) {
    size_t buf_size = 128;
    char* buf = new char[buf_size]{};

    mSerial->begin(baudrate);
    mSerial->flush();
    mSerial->write("AT\r\n");
    return mSerial->readBytes(buf, buf_size) > 0;
}