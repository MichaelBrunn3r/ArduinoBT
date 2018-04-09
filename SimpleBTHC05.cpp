#include "SimpleBTHC05.h"
#include "HC05.h"

SimpleBTHC05::SimpleBTHC05(uint8_t pwrPin, uint8_t keyPin, uint8_t txPin, uint8_t rxPin) {
    // Configure Pins
    mContainer.pwrPin = pwrPin;
    mContainer.keyPin = keyPin;
    pinMode(mContainer.pwrPin, OUTPUT);
    pinMode(mContainer.keyPin, OUTPUT);

    // Configure Serial Connection
    serial = new SoftwareSerial(rxPin,txPin);
    serial->setTimeout(HC05_DEFAULT_SERIAL_TIMEOUT);
    serial->begin(HC05_CMD_MODE_BAUDRATE);

    // Set Serial Parameters
    mContainer.baudrate = 19200;

    // Set initial Mode
    mContainer.mode = HC05Mode::CMD_MODE;
    digitalWrite(mContainer.keyPin, HIGH);
    off();
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