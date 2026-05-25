#pragma once

#include <Arduino.h>

namespace Telemetry {
    struct Data {
        // Sensors
        float volts = 0; // vem de throttle
        float pct = 0;   // vem de throttle

        float temp = NAN;  // vem de dht
        float humi = NAN;  // vem de dht

        float rpm = 0;       // vem de rpm
        float speedKmh = 0;  // vem de rpm

        float currentBat = 0; // vem de current
        float currentMot = 0; // vem de current

        float voltageMin = 0; // vem de throttle
        float voltageMax = 0; // vem de throttle

        float wheelCm = 0;  // vem de rpm
        float ppr = 0;      // vem de rpm

        // Control
        bool overrideEnabled = false; // vem de ramp
        float overridePct = 0;        // vem de ramp

        float maxPct = 0;
    };
}