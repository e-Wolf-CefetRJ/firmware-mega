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
    };

    void defaultValue();
    Data getData();
    Config getConfig();
}