#include <Arduino.h>

#include "ramp.h"
#include "core/telemetry.h"

namespace {
    // Defaults
    constexpr uint16_t RAPID_RAMP_MS    = 250;
    constexpr float    RAPID_UP_PCTPS   = 150.0f;
    constexpr float    SLEW_UP_PCTPS    = 40.0f;
    constexpr float    SLEW_DN_PCTPS    = 60.0f;
    constexpr uint8_t  START_MIN_PCT    = 8;

    // Constantes
    constexpr uint8_t  DEADZONE_PWM        = 4;
    constexpr uint8_t  MIN_START_PWM       = 20;
    constexpr uint16_t REST_DEBOUNCE_MS    = 120;
    constexpr uint16_t SOFT_RAMP_MS        = 1500;
    constexpr uint16_t ACCEL_RAMP_MS       = 250;
    constexpr uint8_t  REST_PCT_THRESHOLD  = 2;

    struct SoftRamp {
        uint32_t  restSinceMs;
        uint32_t  softStartMs;
    } soft;

    struct OutPutRamp {
        float    dutyNowPct;
        float    dutyTargetPct;
        uint32_t rapidUntilMs;
    } output;
    
    struct AccelRamp {
        uint32_t  startMs;
        float     fromPct;
        float     toPct;
        float     lastPedalPct;
    } accel;

    enum class RampMode : uint8_t {
        Accel,
        Rest,
        SoftStart,
        Normal
    };
    RampMode rampMode;
}

// Auxiliares LOOP
void updateRestState(float pedalPct, uint8_t pedalDuty8,uint32_t now) {
    bool belowRest = pedalPct <= REST_PCT_THRESHOLD;

    if (belowRest || pedalDuty8 == 0) {
        if (rampMode != RampMode::Rest)
            soft.restSinceMs = now;

        if ((now - soft.restSinceMs) >= REST_DEBOUNCE_MS) {
            rampMode = RampMode::Rest;
        }
    } else {
        if (rampMode == RampMode::Rest) {
            rampMode = RampMode::SoftStart;
            soft.softStartMs = now;
        }
    }
}

float applySoftRamp(float cappedPct, uint32_t now) {
    if (rampMode != RampMode::SoftStart)
        return cappedPct;

    float rampProgress = (float)(now - soft.softStartMs) / (float)SOFT_RAMP_MS;

    rampProgress = constrain(rampProgress, 0.0f, 1.0f);

    float easedProgress = (3.0f * rampProgress * rampProgress) - (2.0f * rampProgress * rampProgress * rampProgress);

    float maxAllowedPct = easedProgress * 100.0f;

    if (cappedPct > maxAllowedPct) 
        cappedPct = maxAllowedPct;

    if (rampProgress >= 1.0f)
        rampMode = RampMode::Normal;

    return cappedPct;
}

void updateAccelRamp(float pedalPct, uint32_t now, Ramp::Config& config) {
    if (!config.accelSmoothing && rampMode == RampMode::SoftStart)
        return;

    if (pedalPct > accel.lastPedalPct + 0.001f) {
        rampMode = RampMode::Accel;

        accel.startMs = now;
        accel.fromPct = output.dutyNowPct;
        accel.toPct = pedalPct;

    } else if (pedalPct + 0.001f < accel.lastPedalPct) {
        rampMode = RampMode::Normal;
    }

    accel.lastPedalPct = pedalPct;
}

float applyAccelRamp(float cappedPct, uint32_t now) {
    if (rampMode != RampMode::Accel)
        return cappedPct;

    float rampProgress = (float)(now - accel.startMs) / (float)ACCEL_RAMP_MS;

    rampProgress = constrain(rampProgress, 0.0f, 1.0f);

    float easedProgress = (3.0f * rampProgress * rampProgress) - (2.0f * rampProgress * rampProgress * rampProgress);

    float maxAllowedPct = accel.fromPct + easedProgress * (accel.toPct - accel.fromPct);

    if (cappedPct > maxAllowedPct)
        cappedPct = maxAllowedPct;

    if (rampProgress >= 1.0f)
        rampMode = RampMode::Normal;

    return cappedPct;
}

float resolveTarget(float cappedPct, float pedalPct, Ramp::Data& data) {
    if (data.overrideEnable)
        return data.overridePct;

    return max(cappedPct, 
        (pedalPct > 0.0f && pedalPct < START_MIN_PCT)
        ? (float)START_MIN_PCT
        : 0.0f
    );
}

namespace Ramp {
    Config config;
    Data data;

    void defaultValue() {
        config.rapidRamp  =  RAPID_RAMP_MS;
        config.rapidUp    =  RAPID_UP_PCTPS;
        config.slewUp     =  SLEW_UP_PCTPS;
        config.slewDown   =  SLEW_DN_PCTPS;
        config.startMin   =  START_MIN_PCT;
    }


    void start() {
        data.overrideEnable = false;
        data.overridePct = 0;
    }

    void hold(int pct) {
        data.overrideEnable = true;
        data.overridePct = constrain(pct, 0, 100);
        output.dutyTargetPct = pct;
    }

    void stop() {
        data.overrideEnable = true;
        data.overridePct = 0;
        output.dutyTargetPct = 0;
    }


    void setup() {
        uint32_t now = millis();

        rampMode = RampMode::Rest;

        soft.restSinceMs = now;
        soft.softStartMs = 0;

        accel.startMs = 0;
        accel.fromPct = 0.0f;
        accel.toPct = 0.0f;
        accel.lastPedalPct = 0.0f;

        output.dutyNowPct = 0.0f;
        output.dutyTargetPct = 0.0f;
        output.rapidUntilMs = 0;
    }

    void loop(uint32_t now, float cappedPct) {
        
    }
}