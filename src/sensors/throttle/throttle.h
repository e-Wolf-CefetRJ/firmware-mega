#pragma once

#include <Arduino.h>

namespace Throttle {
    struct Config {
        float voltageMin;
        float voltageMax;
    };

    struct Data {
        float volts;
        float pct;
    };

    void defaultValue();
    void loop();

    // Getters
    float getVolts();
    float getPct();
    float getVoltageMin();
    float getVoltageMax();

    // Setters
    void setVoltageMin(float value);
    void setVoltageMax(float value);
}