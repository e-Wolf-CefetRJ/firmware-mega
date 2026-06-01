#include <Arduino.h>

#include "control/pwm.h"
#include "control/ramp.h"

#include "sensors/throttle/throttle.h"
#include "sensors/current/current.h"
#include "sensors/dht/dht.h"
#include "sensors/rpm/rpm.h"

#include "storage/rtc/rtc.h"
#include "storage/eeprom/eeprom.h"

const uint8_t LOOP_MS = 50; // Antes uint32_t
static uint32_t lastLoop = 0;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(30);

    RTC::setup();
    Eeprom::setup();

    PWM::setup();

    Dht::setup();

    Ramp::setup();

    Serial.println(F("ACK:BOOT OK"));
}

void loop() {
    uint32_t now=millis();
    if(now - lastLoop >= LOOP_MS){
        uint32_t dt_ms=now - lastLoop;
        lastLoop=now;

        // PARA FUNCIONAR TEM QUE ESTAR NESTA ORDEM!!
        Throttle::loop();
        // bool idle = (Ramp::getDutyNow()<1.0f) && (RPM::getRpm() < 5.0f));
        Current::loop(); // Idle
        Dht::loop(now);

        float pedalPct = Throttle::getPct();
        Ramp::loop(pedalPct, now, dt_ms); 
        float dutyNowPct = Ramp::getDutyNow();
        PWM::loop(dutyNowPct);
    }
}