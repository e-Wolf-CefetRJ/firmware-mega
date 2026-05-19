#pragma once

#include <Arduino.h>

namespace Eeprom {
    struct Config {
        uint8_t version;
        uint32_t firmwareSignature;    // Assinatura que muda a cada compilação
        
        // Acelerador
        float voltageMin; 
        float voltageMax;
        
        // ?
        uint8_t maxPct;

        // PWM
        uint16_t pwm_hz;
        
        // Rampa
        uint16_t rapid_ms;
        float rapidUp;
        float slewUp;
        float slewDown;
        uint8_t startMin;
    };
    extern Config config;

    void reset();
    void save();
    bool load();
}