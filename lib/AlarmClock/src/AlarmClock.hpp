//
// Created by Malte on 02.10.2023.
//

#ifndef ALARM_CLOCK_STRUCT_H
#define ALARM_CLOCK_STRUCT_H


namespace AlarmClock {

    struct {

        volatile bool anyAlarmTriggered{false};
        float lightLevel{0.0f};
        bool uiActive{false};
        DateTime now{};
        std::array<uint8_t, 6> defuseCode{};
        std::vector<Sound> sounds{};
        Alarm alarm1{N::ONE, preferences};
        Alarm alarm2{N::TWO, preferences};
        uint8_t snoozeTime{0};
        N alarmToSet{N::ONE};
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
            char dateStr[] = " DD. MMM";
            now.toString(dateStr);
            return std::string{dateStr};
        }};
        Player player{preferences};
        UIDisplay ui{OLED_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN};
        const ESP32_Timer uiTimer{
                "UI Timer",
                15000,
                false,
                [this]() { ui.transitionToFrame(0); }
        };

    } AC{};

}

#endif //ALARM_CLOCK_STRUCT_H
