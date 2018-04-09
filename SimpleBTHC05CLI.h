#ifndef SimpleBTHC05CLI_HEADER
#define SimpleBTHC05CLI_HEADER

#include "SimpleBTHC05.h"

namespace SimpleBTHC05CLI {

    void tick(SimpleBTHC05* bt);
    bool sendCmd(SimpleBTHC05* bt, const char* cmd, size_t cmdlen);
}

#endif // SimpleBTHC05CLI_HEADER