#pragma once

#include <Arduino.h>

namespace Ramp {
    struct Config {
        uint16_t rapidRamp; 
        float    rapidUp;
        float    slewUp; 
        float    slewDown; 
        uint8_t  startMin; 

        bool stepMode = false;
        bool accelSmoothing = true;
    };
    extern Config config;

    struct Data {
        bool overrideEnable = false;
        int  overridePct    = 0;
    };
    extern Data data;

    void defaultValue();

    void start();
    void hold(int pct);
    void stop();

    void setup();
    void loop();
}