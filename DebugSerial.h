#ifndef SoftwareLogger_HEADER
#define SoftwareLogger_HEADER

#include "SoftwareSerial.h"

#define DEF_SINGLETON(NAME)         \
    public:                         \
        static NAME* instance() {   \
            static NAME instance;  \
            return &instance;       \
        }                           \
    private:                        \
        NAME(const NAME&);

#ifndef DEBUG_SERIAL_RX
    #define DEBUG_SERIAL_RX 3
#endif
#ifndef DEBUG_SERIAL_TX
    #define DEBUG_SERIAL_TX 2
#endif
#ifndef DEBUG_SERIAL_BAUD
    #define DEBUG_SERIAL_BAUD 38400
#endif

#define LOG(s) DebugSerial::instance()->serial->print(s);
#define LOGN(s) DebugSerial::instance()->serial->println(s);
#define LOGW(s) DebugSerial::instance()->serial->write(s);

class DebugSerial {
    DEF_SINGLETON(DebugSerial)
    
    public:
        SoftwareSerial* serial;
    private:
        DebugSerial () { 
            serial = new SoftwareSerial(DEBUG_SERIAL_RX,DEBUG_SERIAL_TX);
            serial->begin(DEBUG_SERIAL_BAUD);
        } 
};

#endif // SoftwareLogger_HEADER