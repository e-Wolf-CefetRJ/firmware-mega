#pragma once

#include "config/pins.h"

#include <Arduino.h>

namespace PWM {
    void setup();
    void loop(float dutyNowPct);
    
    void defaultValue();

    void setFrequency(uint16_t hz);
    uint16_t getFrequency();
}