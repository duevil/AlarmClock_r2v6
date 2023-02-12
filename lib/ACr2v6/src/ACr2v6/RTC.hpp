//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_RTC_H
#define ALARM_CLOCK_R2V6_RTC_H

class RTC {

public:

    using FiredListener = std::function<void(void)>;

    static const DateTime OFF;

    static void init(const FiredListener &a1 = []() {}, const FiredListener &a2 = []() {}) {

        assert(rtc.begin());
        rtc.disable32K();
        rtc.clearAlarm(A1);
        rtc.clearAlarm(A2);
        pinMode(acc::RTC_INTERRUPT_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(acc::RTC_INTERRUPT_PIN), ISR, FALLING);
        rtc.writeSqwPinMode(DS3231_OFF);
        a1Listener = a1;
        a2Listener = a2;
    }

    inline static void setAlarm1(const DateTime &dt) { setAlarm(A1, dt); };

    inline static void setAlarm2(const DateTime &dt) { setAlarm(A2, dt); };

    inline static void disableAlarm1() { setAlarm1(OFF); }

    inline static void disableAlarm2() { setAlarm2(OFF); }

    inline static DateTime now() { return rtc.now(); }

    inline static void set(const DateTime &dt) { rtc.adjust(dt); }

private:

    enum alarm_t {
        A1 = 1, A2 = 2
    };

    static RTC_DS3231 rtc;
    static FiredListener a1Listener;
    static FiredListener a2Listener;

    static void ISR() {
        if (rtc.alarmFired(A1)) {
            rtc.clearAlarm(A1);
            a1Listener();
        }
        if (rtc.alarmFired(A2)) {
            rtc.clearAlarm(A2);
            a2Listener();
        }
    }

    static void setAlarm(alarm_t alarm, const DateTime &dt) {
        rtc.clearAlarm(alarm);

        if (dt == OFF) {
            rtc.disableAlarm(alarm);
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

const DateTime RTC::OFF{(uint32_t) 0};
RTC_DS3231 RTC::rtc{};
RTC::FiredListener RTC::a1Listener{};
RTC::FiredListener RTC::a2Listener{};


DateTime ac_time::now() { return RTC::now(); }


#endif //ALARM_CLOCK_R2V6_RTC_H
