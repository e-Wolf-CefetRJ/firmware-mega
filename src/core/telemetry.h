#pragma once

#include <Arduino.h>

namespace Telemetry {
    struct Data {
        float volts = 0; // vem de throttle
        float pct = 0;   // vem de throttle

        float temp = NAN;  // vem de dht
        float humi = NAN;  // vem de dht

        float rpm = 0;
        float speedKmh = 0;

        float currentBat = 0; // vem de current
        float currentMot = 0; // vem de current

        float voltageMin = 0; // vem de throttle
        float voltageMax = 0; // vem de throttle

        float wheelCm = 0;
        float ppr = 0;

        bool overrideEnabled = false; // vem de ramp
        float overridePct = 0;        // vem de ramp

        float maxPct = 0;
    };
    extern Data data;

    void getData();
}