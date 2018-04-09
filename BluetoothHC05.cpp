#include "Arduino.h"
#include "BluetoothHC05.h"
#include "SoftwareSerial.h"

/////////////////
// Constructor/Destructor //
/////////////////

BluetoothHC05::BluetoothHC05(uint8_t pwrPIN, uint8_t keyPIN, uint8_t txPIN, uint8_t rxPIN) {
    // Configure Pins
    mPwrPIN = pwrPIN;
    mKeyPIN = keyPIN;
    pinMode(mPwrPIN, OUTPUT);
    pinMode(mKeyPIN, OUTPUT);

    // Configure Serial Connection
    mSerial = new SoftwareSerial(rxPIN,txPIN);
    setTimeout(50);

    mMode = AT_COMMAND_MODE;
    changeMode(COMMUNICATION_MODE);
}

// BluetoothHC05::~BluetoothHC05();

//////////////////////
// Private Methods //
//////////////////////


/////////////////////
// Public Methods //
/////////////////////

bool BluetoothHC05::isOn(bool checkHardware) {
    if(checkHardware) mIsOn = digitalRead(mPwrPIN);
    return mIsOn;
}

void BluetoothHC05::on() {
    digitalWrite(mPwrPIN, HIGH);
    mIsOn = true;
}

void BluetoothHC05::off() {
    digitalWrite(mPwrPIN, LOW);
    mIsOn = false;
}

void BluetoothHC05::setTimeout(uint16_t timeout) {
    mSerial->setTimeout(timeout);
}

size_t BluetoothHC05::write(uint8_t byte) {
    return mSerial->write(byte);
}

size_t BluetoothHC05::write(const char* str) {
    return mSerial->write(str);
}

size_t BluetoothHC05::write(const uint8_t* buffer, size_t size) {
    return mSerial->write(buffer, size);
}

size_t BluetoothHC05::read() {
    return mSerial->read();
}

void BluetoothHC05::changeMode(Mode newMode) {
    // Don't do anything if already in the correct mode
    if(mMode == newMode) return;

    // To switch to or from AT_COMMAND_MODE the HC05 Module has to be restarted
    if(mMode == AT_COMMAND_MODE || newMode == AT_COMMAND_MODE) off();

    /**
     * AT_COMMAND_MODE: Key = HIGH
     * COMMUNICATION_MODE: Key = LOW
     * AT_COMMUNICATION_MODE: Key = HIGH
     */
    if(newMode == AT_COMMAND_MODE) digitalWrite(mKeyPIN, HIGH);
    else digitalWrite(mKeyPIN, LOW); 

    if(mMode == AT_COMMAND_MODE || newMode == AT_COMMAND_MODE) {
        delay(BluetoothHC05_RESTART_TIMEOUT);
        on();
    }

    if(newMode == AT_COMMUNICATION_MODE) {
        if(mMode == AT_COMMAND_MODE) delay(BluetoothHC05_AT_COM_MODE_KEY_DELAY);
        digitalWrite(mKeyPIN, HIGH);
    }

    mMode = newMode;
}

uint16_t BluetoothHC05::determineBaud(uint16_t guess) {
    digitalWrite(mKeyPIN, HIGH);

    // The rates that will be tested
    uint16_t rates[] = {guess,38400,9600,115200,75880,57600,19200,4800,2400,1200,300};
    size_t num_rates = sizeof(rates)/sizeof(uint16_t);

    size_t buf_size = 128;
    char* buf = new char[buf_size]{};

    // Test the Baud rates
    for(int i=0; i<num_rates; i++) {
        uint16_t rate = rates[i];
        mSerial->begin(rate);
        mSerial->flush();
        write("AT\r\n");
        if(mSerial->readBytes(buf, buf_size) > 0) {
            return rate;
        }
    }

    return NO_BAUD_RATE_FOUND;
}