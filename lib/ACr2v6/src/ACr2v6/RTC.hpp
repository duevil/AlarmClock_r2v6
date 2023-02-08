//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_RTC_H
#define ALARM_CLOCK_R2V6_RTC_H


#include "timeAC.h"

using AlarmListener = std::function<void(const uint8_t)>;

class AC_RTC {

public:

    static const DateTime OFF;

    static void init() {
        assert(rtc.begin());
        rtc.disable32K();
        rtc.clearAlarm(A1);
        rtc.clearAlarm(A2);
        pinMode(INTERRUPT_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), ISR, FALLING);
        rtc.writeSqwPinMode(DS3231_OFF);
        DateTime compileDT{__DATE__, __TIME__};
        if (abs((compileDT - now()).totalseconds()) > 60) set(compileDT);
    }

    inline static void setAlarm1(const DateTime &dt) { setAlarm(A1, dt); };

    inline static void setAlarm2(const DateTime &dt) { setAlarm(A2, dt); };

    inline static DateTime now() { return rtc.now(); }

    inline static void set(const DateTime &dt) { rtc.adjust(dt); }

    inline static void onAlarmFired(AlarmListener const &l) { listener = l; };

private:

    enum Alarm {
        A1 = 1, A2 = 2
    };
    static constexpr uint8_t INTERRUPT_PIN = 34;
    static RTC_DS3231 rtc;
    static AlarmListener listener;

    static void ISR() {
        if (rtc.alarmFired(A1)) {
            rtc.clearAlarm(A1);
            listener(A1);
        }
        if (rtc.alarmFired(A2)) {
            rtc.clearAlarm(A2);
            listener(A2);
        }
    }

    static void setAlarm(Alarm alarm, const DateTime &dt) {
        const auto num = static_cast<const uint8_t>(alarm);
        rtc.clearAlarm(num);

        if (dt == OFF) {
            rtc.disableAlarm(num);
        } else {
            switch (alarm) {
                case A1:
                    rtc.setAlarm1(dt, DS3231_A1_Date);
                    break;
                case A2:
                    rtc.setAlarm2(dt, DS3231_A2_Date);
                    break;
            }
        }
    }

};

/* DEFINITIONS */

const DateTime AC_RTC::OFF{(uint32_t) 0};
RTC_DS3231 AC_RTC::rtc{};
AlarmListener AC_RTC::listener{};


DateTime ac_time::now() { return AC_RTC::now(); }


#endif //ALARM_CLOCK_R2V6_RTC_H
