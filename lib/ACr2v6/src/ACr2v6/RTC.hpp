//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_RTC_H
#define ALARM_CLOCK_R2V6_RTC_H


class RTC final : private Initialisable {

public:

    void init() {
        DEBUG_SIMPLE("RTC initialization start");
        setInit();

        static auto ISR = [this]() { /*todo*/ };

        auto b = rtc.begin();
        assert(b && "Starting RTC failed");
        rtc.disable32K();
        rtc.clearAlarm(1);
        rtc.clearAlarm(2);
        pinMode(acc::RTC_INTERRUPT_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(acc::RTC_INTERRUPT_PIN), []() { ISR(); }, FALLING);
        rtc.writeSqwPinMode(DS3231_OFF);

        DEBUG_SIMPLE("RTC initialization end");
    }

    DateTime now() {
        assert(isInit());
        return rtc.now();
    }

    void set(const DateTime &dt) {
        assert(isInit());
        rtc.adjust(dt);
    }

private:

    RTC_DS3231 rtc{};

};


#endif //ALARM_CLOCK_R2V6_RTC_H
