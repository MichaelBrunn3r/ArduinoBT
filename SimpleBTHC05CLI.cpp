#define DEBUG

#include "SimpleBTHC05CLI.h"
#include "StringUtils.h"
#include "HC05.h"
#include "DebugSerial.h"

void SimpleBTHC05CLI::tick(SimpleBTHC05* bt) {
    if(DebugSerial::instance()->serial->available() > 0) {
        size_t buffer_size = 20;
        char buffer[buffer_size] = {};  
        DebugSerial::instance()->serial->readBytes(buffer, buffer_size);

        //if(buffer[0] == 'z') HC05ATCmdHelper::sendGetAddr<SoftwareSerial>(bt->serial);
        SimpleBTHC05CLI::sendCmd(bt, buffer, buffer_size);
    }
}

bool SimpleBTHC05CLI::sendCmd(SimpleBTHC05* bt, const char* cmd, size_t cmdlen) {
    if(StringUtils::startsWith(cmd, "AT")) {
        bt->serial->write(cmd);
        bt->serial->write("\r\n");
    } else if(StringUtils::startsWith(cmd, "BT+")) {
        char* _cmd = (char*)cmd+3;

        if(StringUtils::equals(_cmd, "ON", 2)) bt->on();
        else if(StringUtils::equals(_cmd, "OFF", 3)) bt->off();
        else if(StringUtils::equals(_cmd, "CMD_MODE", 8)) {
            bt->setMode(HC05Mode::CMD_MODE);
            LOGN("Switching to CMD_MODE")
        } else if(StringUtils::equals(_cmd, "RESTART", 7)) {
            LOGN("Restarting")
            bt->off();
            delay(100);
            bt->on();
        }
        else if(StringUtils::equals(_cmd, "DATA_MODE", 9)) {
            bt->setMode(HC05Mode::DATA_MODE);
            LOGN("Switching to DATA_MODE")
        }
        else if(StringUtils::equals(_cmd, "CMD_DATA_MODE", 13)) {
            bt->setMode(HC05Mode::CMD_DATA_MODE); 
            LOGN("Switching to CMD_DATA_MODE")
        } else return false;
        return true;
    }
    return false;
}