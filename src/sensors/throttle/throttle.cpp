#include "throttle.h"
#include "config/pins.h"
#include "core/telemetry.h"

// Valores Privados
namespace {
    // Defaults
    constexpr float VOLTAGE_MIN = 1.10f;
    constexpr float VOLTAGE_MAX = 4.25f;

    // ADC 
    constexpr float VREF_ADC = 5.0f;
    constexpr float ADC_MAX  = 1023.0f;

    // Fault
    constexpr float VOLTAGE_FAULT_LOW  = 0.30f;
    constexpr float VOLTAGE_FAULT_HIGH = 4.90f;

    // Filtro
    int throttleFiltered = 0;
}


// Funções get Data
float getVoltage() {
    int raw = analogRead(Pins::THROTTLE);

    throttleFiltered = (throttleFiltered*3 + raw*5) / 8;
    float v_adc = (throttleFiltered * VREF_ADC) / ADC_MAX;
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

    void defaultValue() {
        config.voltageMin = VOLTAGE_MIN;
        config.voltageMax = VOLTAGE_MAX;
    }

    void loop() {
        float voltage = getVoltage();
        bool fault = (voltage < VOLTAGE_FAULT_LOW) || (voltage > VOLTAGE_FAULT_HIGH);

        float pedalPct = fault ? 0.0f : getPct(voltage);

        data.volts = voltage;
        data.pct   = pedalPct;
    }
}