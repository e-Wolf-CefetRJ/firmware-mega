#include "core/telemetry.h"

// Sensores
#include "sensors/current/current.h"
#include "sensors/throttle/throttle.h"

// Controle
#include "control/ramp.h"

namespace Telemetry {
    Data data;

    void getData() {
        auto throttleData = Throttle::getData();
        data.volts = throttleData.volts;
        data.pct = throttleData.pct;

        data.humi;
        data.temp;

        data.rpm;
        data.speedKmh;

        bool idle = true; // eventualmente mudar isso
        data.currentBat = Current::getCurrentBattery(idle);
        data.currentMot = Current::getCurrentMotor(idle);
    
        auto throttleConfig = Throttle::getConfig();
        data.voltageMin = throttleConfig.voltageMin;
        data.voltageMax = throttleConfig.voltageMax;
    }
}