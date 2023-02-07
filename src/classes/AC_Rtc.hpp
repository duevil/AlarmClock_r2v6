//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_RTC_H
#define ALARM_CLOCK_R2V6_RTC_H

#include <utility>

#include "RTClib.h"


using AlarmListener = std::function<void(const uint8_t)>;

class AC_RTC {

    enum Alarm {
        A1 = 1, A2 = 2
    };
    static constexpr uint8_t INTERRUPT_PIN{34};
    static RTC_DS3231 rtc;
    static AlarmListener listener;

    
    static /* todo: modifier */ void ISR();
    static void setAlarm(Alarm, const DateTime &);

public:

    static const DateTime OFF;

    static void init();

    inline static void setAlarm1(const DateTime &dt) { setAlarm(A1, dt); };

    inline static void setAlarm2(const DateTime &dt) { setAlarm(A2, dt); };

    inline static DateTime now() { return rtc.now(); }

    inline static void set(const DateTime &dt) { rtc.adjust(dt); }

    inline static void onAlarmFired(AlarmListener l) { listener = std::move(l); };

};

/* DEFINITIONS */

const DateTime AC_RTC::OFF{(uint32_t) 0};
RTC_DS3231 AC_RTC::rtc{};
AlarmListener AC_RTC::listener{};

void AC_RTC::init() {
    rtc.begin();
    rtc.disable32K();
    rtc.clearAlarm(A1);
    rtc.clearAlarm(A2);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), ISR, FALLING);
    rtc.writeSqwPinMode(DS3231_OFF);
    DateTime compileDT{__DATE__, __TIME__};
    if (abs((compileDT - now()).totalseconds()) > 60) set(compileDT);
}

void AC_RTC::setAlarm(AC_RTC::Alarm alarm, const DateTime &dateTime) {
    const auto num = static_cast<const uint8_t>(alarm);
    rtc.clearAlarm(num);

    if (dateTime == OFF) {
        rtc.disableAlarm(num);
    } else {
        switch (alarm) {
            case A1:
                rtc.setAlarm1(dateTime, DS3231_A1_Date);
                break;
            case A2:
                rtc.setAlarm2(dateTime, DS3231_A2_Date);
                break;
        }
    }
}

void AC_RTC::ISR() {
    if (rtc.alarmFired(A1)) {
        rtc.clearAlarm(A1);
        listener(A1);
    }
    if (rtc.alarmFired(A2)) {
        rtc.clearAlarm(A2);
        listener(A2);
    }
}


#define AC_TIME_NOW AC_RTC::now()


#endif //ALARM_CLOCK_R2V6_RTC_H
