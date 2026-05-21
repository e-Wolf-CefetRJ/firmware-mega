#include "throttle.h"
#include "../hal/hal_adc.h"
#include "config/pins.h"

// Valores Privados
namespace {
    // Defaults
    constexpr float VOLTAGE_MIN = 1.10f;
    constexpr float VOLTAGE_MAX = 4.25f;

    // Fault
    constexpr float VOLTAGE_FAULT_LOW  = 0.30f;
    constexpr float VOLTAGE_FAULT_HIGH = 4.90f;

    struct InternalData {
        float filtered = 0.0f;
    } internal;
}


// Funções get Data
float getVoltage(uint8_t pin, InternalData& internal) {
    int raw = analogRead(pin);

    internal.filtered = (internal.filtered*3 + raw*5) / 8;
    float v_adc = (internal.filtered * HAL::Adc::VREF_VOLTS) / HAL::Adc::MAX_VALUE;
    return v_adc;
}

float getPct(float v, Throttle::Config& config) {
    float pct = (v - config.voltageMin) / (config.voltageMax - config.voltageMin);

    if (!isfinite(pct))
        pct = 0.0f;

    return constrain(pct, 0.0f, 1.0f) * 100.0f; // Valor percentual
}

namespace Throttle {
    Config config;
    
    void defaultValue() {
        config.voltageMin = VOLTAGE_MIN;
        config.voltageMax = VOLTAGE_MAX;
    }

    Data getData() {
        float voltage = getVoltage(Pins::THROTTLE, internal);
        bool fault = (voltage < VOLTAGE_FAULT_LOW) || (voltage > VOLTAGE_FAULT_HIGH);

        float pedalPct = fault ? 0.0f : getPct(voltage, config);

        return {voltage,pedalPct};
    }

    Config getConfig() {
        return {config.voltageMin,config.voltageMax};
    }
}