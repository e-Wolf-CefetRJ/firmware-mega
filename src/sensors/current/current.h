#pragma once

#include <Arduino.h>

namespace Current {
    // idle = (dutyNowPct < 1.0f) && (rpm < 5.0f); pegar isso de fora e trazer pra cá
    float getCurrentBattery(bool idle);
    float getCurrentMotor(bool idle);
}