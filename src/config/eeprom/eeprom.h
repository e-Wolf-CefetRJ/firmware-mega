#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "sensors/throttle/throttle.h"
#include "control/ramp.h"
#include "control/pwm.h"

namespace Eeprom {
    void reset();
    void save();
    bool load();

    void setup();
}