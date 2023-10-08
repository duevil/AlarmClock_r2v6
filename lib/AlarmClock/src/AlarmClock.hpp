//
// Created by Malte on 02.10.2023.
//

#ifndef ALARM_CLOCK_STRUCT_H
#define ALARM_CLOCK_STRUCT_H


namespace AlarmClock {

    struct {

        float lightLevel{0.0f};
        DateTime now{};
        StringBean tz{"timeZone", preferences};
        Preferences preferences{};
        RTC_DS3231 rtc{};
        AsyncWebServer server{SERVER_PORT};
        LightSensor lightSensor{};
        LEDC indicatorLight{INDICATOR_LED_PIN, LEDC::Resolution::BITS_8};
        MainLight mainLight{preferences};
        Matrix32x8 matrix{SPI_CS_PIN, [this]() {
            return numToStr(now.hour()) + ':' +
                   numToStr(now.minute()) + " " +
                   numToStr(now.second(), 2, true);
        }, [this]() {
            return numToStr(lightLevel, 4) + " lx";
        }, [this]() {
            char dateStr[] = " DD. MMM";
            now.toString(dateStr);
            return std::string{dateStr};
        }};
        Player player{preferences};
        ESP32_Timer rtcTimer{
                "RTC Timer",
                1000,
                true,
                [this]() { now = rtc.now(); }
        };
        std::vector<Sound> sounds{};
        Alarm alarm1{N::ONE, preferences};
        Alarm alarm2{N::TWO, preferences};
        volatile bool anyAlarmTriggered{false};

    } AC{};

}

#endif //ALARM_CLOCK_STRUCT_H
