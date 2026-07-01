#include "throttle.h"

// Valores Privados
namespace {
    // ==================================================
    // ---------------------- CORE ----------------------
    // ==================================================
    struct Config {
        float voltageMin = 0.0f;
        float voltageMax = 0.0f;
    };

    struct Data {
        float volts = 0.0f;
        float pct = 0.0f;
    };

    struct InternalData {
        float filtered = 0.0f;
    };

    Config config;
    Data data;
    InternalData internal;

    // Defaults
    constexpr float VOLTAGE_MIN = 0.87f; 
    constexpr float VOLTAGE_MAX = 4.35f; 

    // ==================================================
    // --------------------- FAULT ----------------------
    // ==================================================
    struct FaultConfig {
        uint8_t debounceCount = 5;      // 5 vezes seguidas em fault -> Desacelera gradualmente
        float reductionStep = 10.0f;    // Desacelera 10%
    };

    struct FaultState {
        bool active = false;
        uint8_t counter = 0;
        float safePct = 0.0f;
    };

    FaultConfig faultConfig;
    FaultState faultState;

    float getFaultLow() {
        float range = config.voltageMax - config.voltageMin;
        return config.voltageMin - (0.1 * range); // Atual: 0.87 - 0.348 = 0.522
    }
    float getFaultHigh() {
        float range = config.voltageMax - config.voltageMin;
        return config.voltageMax + (0.1 * range); // Atual: 4.35 + 0.348 = 4.698
    }

    bool isFault(float voltage) {
        return (voltage < getFaultLow()) || (voltage > getFaultHigh());
    }

    float applyFaultHandler(float voltage, float pct) {
        bool criticalFault = voltage < 0.05f;
        bool fault = isFault(voltage);

        // Debounce
        if (fault) {
            if (faultState.counter < 255)
                faultState.counter++;
        } else {
            faultState.counter = 0;
            faultState.active = false;
            faultState.safePct = pct;
            return pct;
        }

        // Fault ativo só depois de 5 leituras
        if (faultState.counter >= faultConfig.debounceCount) {
            faultState.active = true;
        }

        // Fault crítico, retorna zero sem desaceleração gradual
        if (criticalFault) {
            faultState.active = true;
            faultState.safePct = 0;
            return 0;
        }

        // Redução gradual
        if (faultState.active) {
            faultState.safePct -= faultConfig.reductionStep;

            // Última redução
            if (faultState.safePct < faultConfig.reductionStep)
                faultState.safePct = 0;
        } else {
            faultState.safePct = pct;
        }

        return faultState.safePct;
    }

    // ==================================================
    // ---------------------- MAIN ----------------------
    // ==================================================

    float readFilteredVoltage(uint8_t pin) {
        int raw = analogRead(pin);

        if (internal.filtered == 0) {
            internal.filtered = raw;
        } else {
            // Filtragem: 37,5% do último valor + 62,5% do valor atual
            internal.filtered = (internal.filtered*3 + raw*5) / 8;
        }
        
        // Conversor análogico digital
        float v_adc = (internal.filtered * HAL::Adc::VREF_VOLTS) / HAL::Adc::MAX_VALUE;

        return v_adc;
    }

    float voltageToPct(float v) {
        float pct = (v - config.voltageMin) / (config.voltageMax - config.voltageMin);

        if (!isfinite(pct))
            pct = 0.0f;

        return constrain(pct, 0.0f, 1.0f) * 100.0f;
    }

    void setData() {
        float voltage = readFilteredVoltage(Pins::THROTTLE);
        
        float rawPct = voltageToPct(voltage);
        float pedalPct = applyFaultHandler(voltage,rawPct);
        
        data.volts = voltage;
        data.pct = pedalPct;
    }
}

namespace Throttle {
    // Main
    void loop() {
        setData();
    }

    // Getters
    float getVolts() {
        return data.volts;
    }

    float getPct() {
        return data.pct;
    }

    float getVoltageMin() {
        return config.voltageMin;
    }

    float getVoltageMax() {
        return config.voltageMax;
    }

    // Setters
    void setVoltageMin(float value) {
        config.voltageMin = value;
    }
    
    void setVoltageMax(float value) {
        config.voltageMax = value;
    }

    // Default
    void defaultValue() {
        setVoltageMin(VOLTAGE_MIN);
        setVoltageMax(VOLTAGE_MAX);
    }
}