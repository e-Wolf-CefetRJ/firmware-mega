#pragma once

#include <Arduino.h>

namespace Current {
    struct Data {
        float currentBat;
        float currentMot;
    };
    extern Data data;
    // idle = (dutyNowPct < 1.0f) && (rpm < 5.0f); pegar isso de fora e trazer pra cá
    void loop(uint8_t batteryPin, uint8_t motorPin, bool idle);
}