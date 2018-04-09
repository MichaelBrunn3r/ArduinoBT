#ifndef BluetoothHC05CLI_HEADER
#define BluetoothHC05CLI_HEADER

#include "BluetoothHC05.h"

namespace BluetoothHC05CLI {
    void sendCmd(BluetoothHC05* bt, const char* cmd, size_t cmdlen);
}

#endif // BluetoothHC05CLI_HEADER