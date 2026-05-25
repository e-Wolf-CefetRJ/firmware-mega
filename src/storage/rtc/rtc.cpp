#include "rtc.h"

namespace RTC {
    ThreeWire rtcWire(Pins::RTC_DATA, Pins::RTC_CLOCK, Pins::RTC_RESET);
    RtcDS1302<ThreeWire> rtc(rtcWire);
    bool rtcFine = false;

    void setup() {
        rtc.Begin();

        if (!rtc.GetIsRunning()) {
            rtc.SetIsRunning(true);
            delay(100);
        }

        // Verifica se a data/hora é válida
        if (!rtc.IsDateTimeValid()) {
            // sincroniza automaticamente com horário de compilação
            RtcDateTime compileTime(__DATE__, __TIME__);
            rtc.SetDateTime(compileTime);
        }

        rtcFine = rtc.IsDateTimeValid();
    }

    void printTimestamp(File &f) {
        if (rtcFine) {
            RtcDateTime now = rtc.GetDateTime();
            char buf[20];

            // yyyy-mm-dd HH:MM:SS
            snprintf(buf, sizeof(buf),
            "%04u-%02u-%02u %02u:%02u:%02u",
            now.Year(), now.Month(), now.Day(),
            now.Hour(), now.Minute(), now.Second()
            );
            f.print(buf);
        } else {
            f.print(millis());
        }
    }
}