#include "throttle.h"
#include "../hal/hal_adc.h"

// Valores Privados
namespace {
    // Defaults
    constexpr float VOLTAGE_MIN = 1.10f;
    constexpr float VOLTAGE_MAX = 4.25f;

    // Fault
    constexpr float VOLTAGE_FAULT_LOW  = 0.30f;
    constexpr float VOLTAGE_FAULT_HIGH = 4.90f;
}


// Funções get Data
float getVoltage(uint8_t pin, Throttle::Data& data) {
    int raw = analogRead(pin);

    data.filtered = (data.filtered*3 + raw*5) / 8;
    float v_adc = (data.filtered * HAL::Adc::VREF_VOLTS) / HAL::Adc::MAX_VALUE;
    return v_adc;
}

float getPct(float v) {
    float pct = (v - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN);

    if (!isfinite(pct))
        pct = 0.0f;

    return constrain(pct, 0.0f, 1.0f) * 100.0f; // Valor percentual
}

namespace Throttle {
    Config config;
    Data data;

    void defaultValue() {
        config.voltageMin = VOLTAGE_MIN;
        config.voltageMax = VOLTAGE_MAX;
    }

    void loop(uint8_t pin) {
        float voltage = getVoltage(pin, data);
        bool fault = (voltage < VOLTAGE_FAULT_LOW) || (voltage > VOLTAGE_FAULT_HIGH);

        float pedalPct = fault ? 0.0f : getPct(voltage);

        data.volts = voltage;
        data.pct   = pedalPct;
    }
}