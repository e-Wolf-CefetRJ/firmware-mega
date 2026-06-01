#pragma once

#include <Arduino.h>
#include "config/pins.h"

namespace RPM {
    void setup();
    void loop();

    float getWheelCm();
    uint8_t getPpr();

    float getRpm();
    float getSpeed();

    void setWheelCm(float value);
    void setPpr(uint8_t value);
}