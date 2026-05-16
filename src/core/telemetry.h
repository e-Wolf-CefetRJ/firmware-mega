#pragma once

#include <Arduino.h>

struct Data {
    float volts = 0;
    float pct = 0;

    float temp = NAN;
    float humi = NAN;

    float rpm = 0;
    float speedKmh = 0;

    float currentBat = 0;
    float currentMot = 0;

    // Vem de outros lugares, trazer valores pra cá
    float voltageMin = 0;
    float voltageMax = 0;

    float wheelCm = 0;
    float ppr = 0;

    bool overrideEnabled = false;
    float overridePct = 0;
    
    float maxPct = 0;
};

extern Data data;