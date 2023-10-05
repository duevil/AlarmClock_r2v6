//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_H
#define ALARM_CLOCK_MAIN_H


namespace AlarmClock {

    /**
     * The main setup of the alarm clock; sets up the alarm clock and all its components
     */
    void setup() {
        Serial.begin(SERIAL_BAUD);
        bootProgress(BootState::Start);

        AC.preferences.begin(PREFERENCES_NAMESPACE);
        AC.mainLight.setup();

        bootProgress(BootState::Matrix);
        AC.matrix.setup();

        bootProgress(BootState::RTC);
        rtc::setup();
        AC.rtcTimer.start();

        bootProgress(BootState::Navigation);
        navigation::setup();

        bootProgress(BootState::WiFi);
        esp32_wifi::setup([]() { bootProgress(BootState::SmartConfig); });

        bootProgress(BootState::NTP);
        AC.tz.load();
        esp32_wifi::setupNTP([](struct timeval *) { rtc::adjustTimeFromInternalRTC(); }, ((String) AC.tz).c_str());

        bootProgress(BootState::Webserver);
        webserver::setup();

        bootProgress(BootState::LightSensor);
        AC.lightSensor.setup();

        bootProgress(BootState::LEDC);
        AC.indicatorLight.setup();

        bootProgress(BootState::Player);
        AC.player.setup();

        bootProgress(BootState::Done);
    }

    /**
     * The main loop of the alarm clock
     */
    void loop() {
        static auto &matrix = AC.matrix;
        static auto &lightLevel = AC.lightLevel;
        static auto &mainLight = AC.mainLight;
        static bool matrixIlluminate{false};
        static ESP32_Timer matrixScrollTimer{
                "Matrix Scroll Timer",
                2000,
                false,
                []() { matrix.scrollToStart(); }
        };
        static ESP32_Timer matrixIlluminateTimer{
                "Matrix Illuminate Timer",
                5000,
                false,
                []() { matrixIlluminate = false; }
        };

        matrix.loop();

        // callback to illuminate the matrix
        auto illuminateMatrix = []() {
            matrixIlluminate = true;
            matrixIlluminateTimer.start();
        };

        // handle light sensor and matrix illumination
        if (AC.lightSensor.tryReading()) {
            auto value = AC.lightSensor.getValue();
            if (lightLevel != 0 && value == 0) illuminateMatrix();
            lightLevel = value;
            matrix.setBrightness((uint8_t)(0.1005 * lightLevel - 0.05));
            matrix.shutdown(lightLevel == 0 && !matrixIlluminate && !mainLight.getDuty());
        }

        // TODO: implement settings mode
        auto insideSettings = false;

        // handle navigation actions
        auto navDir = navigation::read();
        if (!matrixIlluminate && navDir != navigation::Direction::None && lightLevel < 1e-5) {
            illuminateMatrix();
        } else if (insideSettings) {
            // TODO: implement UI navigation
        } else {
            switch (navDir) {
                case navigation::Direction::Center:
                    if (AC.indicatorLight.getDuty()) AC.indicatorLight.toggleOff();
                    else AC.indicatorLight.toggleOn();
                    break;
                case navigation::Direction::Left:
                    matrix.scrollPrev();
                    matrixScrollTimer.start();
                    break;
                case navigation::Direction::Right:
                    matrix.scrollNext();
                    matrixScrollTimer.start();
                    break;
                case navigation::Direction::Up:
                    mainLight.incrDuty();
                    if (!mainLight.getDuty()) illuminateMatrix();
                    break;
                case navigation::Direction::Down:
                    mainLight.decrDuty();
                    if (!mainLight.getDuty()) illuminateMatrix();
                    break;
                case navigation::Direction::None:
                    // do nothing
                    break;
            }
        }
    }

}


#endif //ALARM_CLOCK_MAIN_H
