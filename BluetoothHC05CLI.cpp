#include "BluetoothHC05CLI.h"
#include "StringUtils.h"
  
bool BluetoothHC05CLI::sendCmd(BluetoothHC05* bt, const char* cmd, size_t cmdlen) {
    if(StringUtils::startsWith(cmd, "AT")) { 
        bt->serial()->write(cmd);
        bt->serial()->write("\r\n");
    } else if(StringUtils::startsWith(cmd, "BT+")) {
        char* _cmd = (char*)cmd+3;

        if(StringUtils::equals(_cmd, "ON", 2)) bt->on();
        else if(StringUtils::equals(_cmd, "OFF", 3)) bt->off();
        else if(StringUtils::equals(_cmd, "CMD_MODE", 8)) bt->setMode(BluetoothHC05::CMD_MODE);
        else if(StringUtils::equals(_cmd, "DATA_MODE", 9)) bt->setMode(BluetoothHC05::DATA_MODE);
        else if(StringUtils::equals(_cmd, "CMD_DATA_MODE", 13)) bt->setMode(BluetoothHC05::CMD_DATA_MODE); 
        else if(StringUtils::equals(_cmd, "test", 4)) bt->setSerialParameters(9600, 0, 0, true);
        else return false;
    } else return false;
    return true;
}