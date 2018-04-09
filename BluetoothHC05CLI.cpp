#include "BluetoothHC05.h"
#include "BluetoothHC05CLI.h"

bool startsWith(const char* str, const char* prefix) {
    uint8_t strChar, prefixChar;
    const uint8_t* _prefix = (uint8_t*) prefix;
    const uint8_t* _str = (uint8_t*) str;
    while ( ( strChar = *_str++ ) & ( prefixChar = *_prefix++ ) ) {
        if ( prefixChar != strChar ) return false;
    }
    return !prefixChar;
}

bool equals(const char* str1, const char* str2, size_t strlen) {
    for(int i=0; i<strlen; i++) if(str1[i] != str2[i]) return false;
    return true;
}
  
void BluetoothHC05CLI::sendCmd(BluetoothHC05* bt, const char* cmd, size_t cmdlen) {
    if(startsWith(cmd, "AT")) { 
        bt->writeATCmd(cmd);
    } else if(startsWith(cmd, "BT+")) {
        char* _cmd = (char*)cmd+3;

        if(equals(_cmd, "ON", 2)) bt->on();
        else if(equals(_cmd, "OFF", 3)) bt->off();
        else if(equals(_cmd, "CMD_MODE", 8)) bt->changeMode(BluetoothHC05::CMD_MODE);
        else if(equals(_cmd, "DATA_MODE", 9)) bt->changeMode(BluetoothHC05::DATA_MODE);
        else if(equals(_cmd, "CMD_DATA_MODE", 13)) bt->changeMode(BluetoothHC05::CMD_DATA_MODE); 
    }
}