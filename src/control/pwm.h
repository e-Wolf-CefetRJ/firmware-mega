#pragma once

#include "config/pins.h"

#include <Arduino.h>

namespace PWM {
    struct Config {
        uint16_t frequency; // 100 até 8000
    };

    void setup();

    void applyFrequency();
    
    void defaultValue();

    void setFrequency(uint16_t hz);
    void setPct(float pct);

    uint16_t getFrequency();
}