#pragma once

#include <Arduino.h>

namespace Throttle {
    struct Config {
        float voltageMin;
        float voltageMax;
    };
    extern Config config;

    struct Data {
        float volts;
        float pct;
        int filtered;
    };
    extern Data data;

    void defaultValue();
    void loop(uint8_t pin);
}