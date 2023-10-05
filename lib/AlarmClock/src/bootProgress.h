//
// Created by Malte on 05.10.2023.
//

#ifndef ALARM_CLOCK_BOOT_PROGRESS_H
#define ALARM_CLOCK_BOOT_PROGRESS_H


namespace AlarmClock {

    enum class BootState : uint8_t {
        Start = 0,
        Matrix = 1,
        RTC = 5,
        Navigation = 11,
        WiFi = 16,
        SmartConfig = 22,
        NTP = 31,
        Webserver = 36,
        LightSensor = 41,
        LEDC = 46,
        Player = 51,
        Done = 100,
    };


    /**
     * Handles the boot progress
     * @param state The current boot state
     * @return The current boot state
     */
    BootState bootProgress(BootState state) {
        switch (state) {
            case BootState::Start:
                Serial.println("Booting");
                break;
            case BootState::Matrix:
                Serial.println("Matrix");
                break;
            case BootState::RTC:
                AC.matrix.overrideText("Booting");
                Serial.println("RTC");
                break;
            case BootState::Navigation:
                Serial.println("Navigation");
                break;
            case BootState::WiFi:
                Serial.println("WiFi");
                break;
            case BootState::SmartConfig:
                Serial.println("SmartConfig");
                AC.matrix.overrideText("smcnfg");
                break;
            case BootState::NTP:
                AC.matrix.overrideText("Booting");
                Serial.println("NTP");
                break;
            case BootState::Webserver:
                Serial.println("Webserver");
                break;
            case BootState::LightSensor:
                Serial.println("LightSensor");
                break;
            case BootState::Done:
                Serial.println("Done");
                break;
            case BootState::LEDC:
                Serial.println("LEDC");
                break;
            case BootState::Player:
                Serial.println("Player");
                break;
        }
        return state;
    }

}


#endif //ALARM_CLOCK_BOOT_PROGRESS_H
