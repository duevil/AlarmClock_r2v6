//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_H
#define ALARM_CLOCK_MAIN_H


namespace AlarmClock {

    void alarmHandler() {
        static ESP32_Timer turnOffTimer{
                "Turn Off Timer",
                30 * 60 * 1000 /* 30 min */,
                false,
                []() {
                    AC.player.stop();
                    AC.indicatorLight.toggleOff();
                }
        };
        if (readAlarm(AC.alarm1, AC.rtc)) {
            auto sound = (uint8_t) AC.alarm1.sound;
            if (sound == 0) AC.player.playLoop(Sound::findRandomSound(AC.sounds).getId());
            else AC.player.playLoop(sound);
        }
        if (readAlarm(AC.alarm2, AC.rtc)) {
            auto sound = (uint8_t) AC.alarm2.sound;
            if (sound == 0) AC.player.playLoop(Sound::findRandomSound(AC.sounds).getId());
            else AC.player.playLoop(sound);
        }
        turnOffTimer.start();
        AC.indicatorLight.toggleOn();
        AC.mainLight.toggleOn();
    }

    /**
     * The main setup of the alarm clock; sets up the alarm clock and all its components
     */
    void setup() {
        Serial.begin(SERIAL_BAUD);
        bootProgress(BootState::Start);

        assert(SPIFFS.begin() && "SPIFFS failed to mount");

        assert(AC.preferences.begin(PREFERENCES_NAMESPACE));
        AC.mainLight.setup();

        bootProgress(BootState::Matrix);
        assert(AC.matrix.setup() && "Matrix failed to initialize");

        bootProgress(BootState::RTC);
        assert(rtc::setup() && "RTC failed to initialize");
        AC.rtcTimer.start();
        rtc::attachInterrupt(RTC_SQW_PIN, [](){ AC.anyAlarmTriggered = true; });

        bootProgress(BootState::Navigation);
        navigation::setup();

        bootProgress(BootState::WiFi);
        assert(esp32_wifi::setup([]() { bootProgress(BootState::SmartConfig); }) && "WiFi failed to initialize");

        bootProgress(BootState::NTP);
        AC.tz.load();
        esp32_wifi::setupNTP([](struct timeval *) { rtc::adjustTimeFromInternalRTC(); }, ((String) AC.tz).c_str());

        bootProgress(BootState::Webserver);
        webserver::setup();

        bootProgress(BootState::LightSensor);
        assert(AC.lightSensor.setup() && "Light sensor failed to initialize");

        bootProgress(BootState::LEDC);
        AC.indicatorLight.setup();

        bootProgress(BootState::Player);
        assert(AC.player.setup() && "Player failed to initialize");

        AC.sounds = Sound::loadSounds(JSON_SOUNDS_FILE_NAME);

        setupAlarm(AC.alarm1);
        setupAlarm(AC.alarm2);
        setAlarm(AC.alarm1, AC.rtc);
        setAlarm(AC.alarm2, AC.rtc);

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

        // alarm handler
        if (AC.anyAlarmTriggered) {
            alarmHandler();
            AC.anyAlarmTriggered = false;
        }

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
            matrix.setBrightness((uint8_t) (0.1005 * lightLevel - 0.05));
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
                    if (AC.alarm1.state == AlarmState::PLAYING || AC.alarm2.state == AlarmState::PLAYING) {
                        // TODO: implement snooze time setting
                        // TODO: implement snooze ui
                        if (AC.alarm1.state == AlarmState::PLAYING) {
                            AC.alarm1.state = AlarmState::SNOOZE;
                            snoozeAlarm(AC.alarm1, AC.rtc, 5);
                        }
                        if (AC.alarm2.state == AlarmState::PLAYING) {
                            AC.alarm2.state = AlarmState::SNOOZE;
                            snoozeAlarm(AC.alarm2, AC.rtc, 5);
                        }
                        AC.player.stop();
                        AC.indicatorLight.setDuty(AC.indicatorLight.getMaxDuty() / 3);
                    }
                    else if (AC.alarm1.state == AlarmState::SNOOZE || AC.alarm2.state == AlarmState::SNOOZE) {
                        if (AC.alarm1.state == AlarmState::SNOOZE) {
                            AC.alarm1.state = AlarmState::OFF;
                            if ((uint8_t) AC.alarm1.repeat) setAlarm(AC.alarm1, AC.rtc);
                            else disableAlarm(AC.alarm1, AC.rtc);
                        }
                        if (AC.alarm2.state == AlarmState::SNOOZE) {
                            AC.alarm2.state = AlarmState::OFF;
                            if ((uint8_t) AC.alarm2.repeat) setAlarm(AC.alarm2, AC.rtc);
                            else disableAlarm(AC.alarm2, AC.rtc);
                        }
                        AC.indicatorLight.toggleOff();
                    }
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
