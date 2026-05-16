#pragma once

#include <Arduino.h>

struct Data {
    float volts = 0; // vem de throttle
    float pct = 0;   // vem de throttle

    float temp = NAN;
    float humi = NAN;

    float rpm = 0;
    float speedKmh = 0;

    float currentBat = 0; // vem de current
    float currentMot = 0; // vem de current

    float voltageMin = 0; // vem de throttle
    float voltageMax = 0; // vem de throttle

    float wheelCm = 0;
    float ppr = 0;

    bool overrideEnabled = false;
    float overridePct = 0;
    
    float maxPct = 0;
};

extern Data data;