#ifndef HC05ATCmdHelper_HEADER
#define HC05ATCmdHelper_HEADER

#include "Arduino.h"
#include "StringUtils.h"

namespace HC05ATCmdHelper {

    template <typename TSERIAL>
    bool recvATCmdString(TSERIAL* serial, char* buf, size_t bufLen, char* suffix, size_t suffixLen);

    template <typename TSERIAL>
    void sendGetAddr(TSERIAL* serial);

    template <typename TSERIAL>
    bool recvGetAddr(TSERIAL* serial, char* buf, size_t bufLen);
}

#endif // HC05ATCmdHelper