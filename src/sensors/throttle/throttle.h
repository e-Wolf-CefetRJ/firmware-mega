#pragma once

#include <Arduino.h>

namespace Throttle {
    struct Config {
        float voltageMin;
        float voltageMax;
    };
    extern Config config;

    void  defaultValue();
    void loop();
}