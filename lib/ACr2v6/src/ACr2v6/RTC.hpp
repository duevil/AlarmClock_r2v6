//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_RTC_H
#define ALARM_CLOCK_R2V6_RTC_H


using FiredListener = std::function<void(const uint8_t &)>;

class RTC {

public:

    static const DateTime OFF;

    static void init() {
        static constexpr uint8_t MAX_OFFSET{60};

        assert(rtc.begin());
        rtc.disable32K();
        rtc.clearAlarm(A1);
        rtc.clearAlarm(A2);
        pinMode(acc::RTC_INTERRUPT_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(acc::RTC_INTERRUPT_PIN), ISR, FALLING);
        rtc.writeSqwPinMode(DS3231_OFF);
        DateTime compileDT{__DATE__, __TIME__};
        if (abs((compileDT - now()).totalseconds()) > MAX_OFFSET) set(compileDT);
    }

    inline static void setAlarm1(const DateTime &dt) { setAlarm(A1, dt); };

    inline static void setAlarm2(const DateTime &dt) { setAlarm(A2, dt); };

    inline static void disableAlarm1() { setAlarm1(OFF); }

    inline static void disableAlarm2() { setAlarm2(OFF); }

    inline static DateTime now() { return rtc.now(); }

    inline static void set(const DateTime &dt) { rtc.adjust(dt); }

    inline static void onAlarmFired(FiredListener const &l) { listener = l; };

private:

    enum alarm_t {
        A1 = 1, A2 = 2
    };

    static RTC_DS3231 rtc;
    static FiredListener listener;

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
FiredListener RTC::listener{};


DateTime ac_time::now() { return RTC::now(); }


#endif //ALARM_CLOCK_R2V6_RTC_H
