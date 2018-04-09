#include "SimpleBTHC05.h"
#include "HC05.h"
#include "StringUtils.h"

//////////////////////
///  Constructor   ///
//////////////////////

SimpleBTHC05::SimpleBTHC05(uint8_t pwrPin, uint8_t keyPin, uint8_t txPin, uint8_t rxPin, uint32_t baudrate) {
    // Configure Pins
    mContainer.pwrPin = pwrPin;
    mContainer.keyPin = keyPin;
    pinMode(mContainer.pwrPin, OUTPUT);
    pinMode(mContainer.keyPin, OUTPUT);

    // Configure Serial Connection
    serial = new SoftwareSerial(rxPin,txPin);
    serial->setTimeout(HC05_DEFAULT_SERIAL_TIMEOUT);
    serial->begin(HC05_CMD_MODE_BAUDRATE);

    // Set initial Mode
    mContainer.mode = HC05Mode::CMD_MODE;
    digitalWrite(mContainer.keyPin, HIGH);
    on();

    mContainer.baudrate = baudrate;
}

//////////////////////
/// Public Methods ///
//////////////////////

void SimpleBTHC05::setup() {
    setSerialParameters(mContainer.baudrate, HC05_DEFAULT_STOP_BIT, HC05_DEFAULT_PARITY_BIT, true);
}

bool SimpleBTHC05::isOn() {
    return mContainer.isOn;
}

void SimpleBTHC05::on() {
    if(mContainer.mode == HC05Mode::CMD_DATA_MODE) digitalWrite(mContainer.keyPin, LOW);

    digitalWrite(mContainer.pwrPin, HIGH);

    if(mContainer.mode == HC05Mode::CMD_DATA_MODE) {
        delay(HC05_RESTART_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mContainer.keyPin, HIGH);
    }

    mContainer.isOn = true;
}

void SimpleBTHC05::off() {
    digitalWrite(mContainer.pwrPin, LOW);
    mContainer.isOn = false;
}

HC05Mode SimpleBTHC05::getMode() {
    return mContainer.mode; 
}

HC05Mode SimpleBTHC05::setMode(HC05Mode newMode) {
    // Don't do anything if already in the correct mode
    if(mContainer.mode == newMode) return;

    HC05Mode prevMode = mContainer.mode;
    mContainer.mode = newMode;

    // When switching from or to CMD_MODE, the HC05 Module has the be restarted.
    // First step is to turn off the Module.
    if(prevMode == HC05Mode::CMD_MODE || newMode == HC05Mode::CMD_MODE) off();

    // CMD_MODE: Key = HIGH
    // DATA_MODE: Key = LOW
    // CMD_DATA_MODE: Key = HIGH
    if(newMode == HC05Mode::CMD_MODE) digitalWrite(mContainer.keyPin, HIGH);
    else if(prevMode == HC05Mode::CMD_MODE) digitalWrite(mContainer.keyPin, LOW); 

    // When switching to or from CMD_MODE, restart HC05 Module after a short delay
    if(prevMode == HC05Mode::CMD_MODE || newMode == HC05Mode::CMD_MODE) {
        delay(HC05_RESTART_TIMEOUT);
        on();
    }

    if(newMode == HC05Mode::CMD_DATA_MODE) {
        if(prevMode == HC05Mode::CMD_MODE) delay(HC05_RESTART_CMD_DATA_MODE_KEY_DELAY);
        digitalWrite(mContainer.keyPin, HIGH);
    }

    // Switch to Baudrate of the new Mode.
    // CMD_MODE has a fixed Baudrate of BTHC05_CMD_MODE_BAUDRATE (typically 38400).
    // The Baudrate of the other modes is saved in SimpleBTHC05::mContainer.baudrate.
    if(newMode == HC05Mode::DATA_MODE || newMode == HC05Mode::CMD_DATA_MODE) serial->begin(mContainer.baudrate);
    else if(newMode == HC05Mode::CMD_MODE) serial->begin(HC05_CMD_MODE_BAUDRATE);

    // Set new and return old Mode
    
    return prevMode;
}

uint32_t SimpleBTHC05::getBaudrate() {
    return mContainer.baudrate;
}

bool SimpleBTHC05::setSerialParameters(uint32_t baudrate, bool stopBit, bool parityBit, bool checkReply) {
    HC05Mode execMode = mContainer.mode; // The Mode the Module was in when this Method was executed (important for when switching Modes)
    bool success = false;

    size_t atCmdBufSize = 25;
    char* atCmd = new char[atCmdBufSize]{}; // Buffer for the AT Command that will be sent to the Module to change its Serial Parameters
    snprintf(atCmd, atCmdBufSize, "AT+UART=%lu,%u,%u\r\n", baudrate, stopBit, parityBit); // Create the AT Command String

    // Send Command
    if(mContainer.mode == HC05Mode::DATA_MODE) {
        // Can't send commands in DATA_MODE! First switch to CMD_DATA_MODE
        setMode(HC05Mode::CMD_DATA_MODE);
        serial->flush();
        serial->write(atCmd);
    } else serial->write(atCmd);

    // If specified: Check reply to confirm if command was successfull
    if(checkReply && serial->available()) {
        size_t replyLen = 128;
        char* reply = new char[replyLen]{};
        serial->readBytes(reply, replyLen);
        success = StringUtils::contains(reply, "OK") ;
    } else success = true; // Else, assume successfull execution

    // If Module was in DATA_MODE, switch back to it
    if(execMode == HC05Mode::DATA_MODE) setMode(HC05Mode::DATA_MODE);

    // If execution was successfull, save new Serial Parameters
    if(success) {
        mContainer.baudrate = baudrate;

        // The Baudrate of DATA_MODE and CMD_DATA_MODE changed. If Module is currently in one of these modes, change the Serial Baudrate
        if(mContainer.mode == HC05Mode::DATA_MODE || mContainer.mode == HC05Mode::CMD_DATA_MODE) serial->begin(mContainer.baudrate);
    }

    return success;
}