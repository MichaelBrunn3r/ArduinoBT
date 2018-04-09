#include "HC05ATCmdHelper.h"

template <typename TSERIAL>
bool HC05ATCmdHelper::recvATCmdString(TSERIAL* serial, char* buf, size_t bufLen, char* suffix, size_t suffixLen) {
    if(serial->available()) {
        size_t replyLen = bufLen + suffixLen + 3;
        char* reply = new char[replyLen]{};
        serial->readBytes(reply, replyLen);

        Serial.println("Received String: ");
        Serial.println(reply);
        
        if(StringUtils::startsWith(reply, suffix)) {
            int i=suffixLen;
            for(i; i<replyLen; i++) {
                char c = reply[i];
                if(c == ' ') break;
                buf[i] == c;
            }
            return true;
        }
    }
    return false;
}

template <typename TSERIAL>
void HC05ATCmdHelper::sendGetAddr(TSERIAL* serial) {
    serial->write("AT+ADDR?\r\n");
}

template <typename TSERIAL>
bool HC05ATCmdHelper::recvGetAddr(TSERIAL* serial, char* buf, size_t bufLen) {
    size_t addrLen = 17;
    char* addr = new char[addrLen]{};
    if(recvATCmdString<TSERIAL>(serial, addr, addrLen, "+ADDR:", 6)) {
        Serial.println("Address is: ");
        Serial.println(addr);
        return true;
    }
    return false;
}