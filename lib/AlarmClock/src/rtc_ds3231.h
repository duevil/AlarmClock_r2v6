//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_RTC_DS3231_H
#define ALARM_CLOCK_RTC_DS3231_H


namespace AlarmClock {
    namespace rtc {

        /**
         * Sets up the RTC
         * @param rtc The RTC to setup
         * @return True if the RTC was setup successfully, false otherwise
         */
        bool setup(RTC_DS3231 &rtc = AC.rtc) {
            if (!rtc.begin()) return false;
            if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            rtc.clearAlarm(1);
            rtc.clearAlarm(2);
            rtc.writeSqwPinMode(DS3231_OFF);
            return true;
        }

        /**
         * Attaches an ISR to the given pin that triggers when any of the two RTC alarms go off
         * @tparam ISR The ISR type
         * @param pin The pin to attach the ISR to
         * @param isr The ISR (interrupt service routine)
         */
        template<typename ISR>
        void attachInterrupt(uint8_t pin, ISR isr) {
            pinMode(pin, INPUT_PULLUP);
            ::attachInterrupt(digitalPinToInterrupt(pin), isr, FALLING);
        }

        /**
         * Detaches the ISR from the given pin
         * @param pin The pin to detach the ISR from
         */
        void detachInterrupt(uint8_t pin) {
            ::detachInterrupt(digitalPinToInterrupt(pin));
        }

        /**
         * Adjusts the given RTC to the current time from the ESP32's internal RTC
         * @param rtc The RTC to adjust
         * @return True if the RTC was adjusted successfully, false otherwise
         */
        bool adjustTimeFromInternalRTC() {
            struct tm t{};
            // the function uses a 10ms delay after a failed attempt
            // so a timeout of 100ms should be at most 10 attempts
            if (!getLocalTime(&t, 100)) return false;
            DateTime dt{
                    static_cast<uint16_t>(t.tm_year + 1900),
                    static_cast<uint8_t>(t.tm_mon + 1),
                    static_cast<uint8_t>(t.tm_mday),
                    static_cast<uint8_t>(t.tm_hour),
                    static_cast<uint8_t>(t.tm_min),
                    static_cast<uint8_t>(t.tm_sec)
            };
            if (dt.isValid() && abs((AC.now - dt).totalseconds()) > 10) AC.rtc.adjust(dt);
            return true;
        }

    }
}


#endif //ALARM_CLOCK_RTC_DS3231_H
