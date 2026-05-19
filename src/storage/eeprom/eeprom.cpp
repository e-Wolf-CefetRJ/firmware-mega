#include <EEPROM.h>

#include "eeprom.h"
#include "./sensors/throttle/throttle.h"
#include "./control/ramp.h"

// Fowller-Noll-Vo hash function; Cria uma assinatura a cada compilação
// Única forma da assinatura for igual, é se as compilações forem ao mesmo tempo
uint32_t buildSignature() {
    const char* date = __DATE__;
    const char* time = __TIME__;

    // FNV-1a 32-bit: offset basis e prime
    uint32_t hash = 2166136261UL;   // 0x811c9dc5
    const uint32_t prime = 16777619UL; // 0x01000193

    while (*date) {
        hash ^= (uint8_t)*date++;
        hash *= prime;
    }

    hash ^= (uint8_t)'|';
    hash *= prime;

    while (*time) {
        hash ^= (uint8_t)*time++;
        hash *= prime;
    }

    return hash;
}

namespace Eeprom{
    Config config;
    
    void reset() {
        Throttle::defaultValue(); // voltageMin e voltageMax
        Ramp::defaultValue();
    }

    void save() {
        config.firmwareSignature = buildSignature();

        config.voltageMin  =  Throttle::config.voltageMin;
        config.voltageMax  =  Throttle::config.voltageMax;

        config.rapid_ms  =   Ramp::config.rapidRamp;
        config.rapidUp   =   Ramp::config.rapidUp;
        config.slewUp    =   Ramp::config.slewUp;
        config.slewDown  =   Ramp::config.slewDown;
        config.startMin  =   Ramp::config.startMin;

        EEPROM.put(0,config);
    }

    bool load() {
        EEPROM.get(0,config);

        //if (config.version != ) return false;
        if (config.firmwareSignature != buildSignature()) return false;

        Throttle::config.voltageMin = config.voltageMin;
        Throttle::config.voltageMax = config.voltageMax;

        Ramp::config.rapidRamp  =  config.rapid_ms;
        Ramp::config.rapidUp    =  config.rapidUp;
        Ramp::config.slewUp     =  config.slewUp;
        Ramp::config.slewDown   =  config.slewDown;
        Ramp::config.startMin   =  config.startMin;

        return true;
    }
}