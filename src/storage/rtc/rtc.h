#pragma once

#include <RtcDS1302.h>
#include <ThreeWire.h>
#include <SD.h> 

#include "config/pins.h"

// Real-Time Clock
namespace RTC {
    void setup();
    void printTimestamp(File &f);
}