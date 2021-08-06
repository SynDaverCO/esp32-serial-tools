
#pragma once

#include "SerialPort.h"

class BOSSA {
    public:
        static void flash(File &firmware, SerialPort::Ptr port);
};