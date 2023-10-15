//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_H
#define ALARM_CLOCK_MAIN_H

#define FRAME_CALLBACK(f) [](OLEDDisplay*d,OLEDDisplayUiState*s,int16_t x,int16_t y){f({d,s,x,y});}


namespace AlarmClock {

    /**
     * The main setup of the alarm clock; sets up the alarm clock and all its components
     */
    void setup() {
        static const ESP32_Timer rtcTimer{
                "RTC Timer",
                1000,
                true,
                []() {
                    auto dt = AC.rtc.now();
                    if (dt.isValid()) AC.now = dt;
                }
        };

        auto &ui = AC.ui;
        ui.setup();
        ui.drawBootAnimation(0, "Booting");

        ui.drawBootAnimation(5, "Mounting SPIFFS");
        assert(SPIFFS.begin() && "SPIFFS failed to mount");

        ui.drawBootAnimation(10, "Loading preferences");
        assert(AC.preferences.begin(PREFERENCES_NAMESPACE));

        ui.drawBootAnimation(15, "Initializing Matrix");
        assert(AC.matrix.setup() && "Matrix failed to initialize");
        AC.matrix.overrideText("Booting");

        ui.drawBootAnimation(20, "Initializing RTC");
        assert(rtc::setup() && "RTC failed to initialize");
        rtcTimer.start();
        rtc::attachInterrupt(RTC_SQW_PIN, []() { AC.anyAlarmTriggered = true; });

        ui.drawBootAnimation(25, "Initializing Touchpad");
        navigation::setup();

        ui.drawBootAnimation(35, "Connecting WiFi");
        esp32_wifi::setup([]() { ui.drawBootAnimation(40, "Running SmartConfig"); });
        AC.tz.load();
        esp32_wifi::setupNTP([](struct timeval *) { rtc::adjustTimeFromInternalRTC(); }, ((String) AC.tz).c_str());

        ui.drawBootAnimation(45, "Initializing Webserver");
        webserver::setup();

        ui.drawBootAnimation(50, "Initializing Light Sensor");
        assert(AC.lightSensor.setup() && "Light sensor failed to initialize");

        ui.drawBootAnimation(55, "Initializing Main Light");
        AC.mainLight.setup();

        ui.drawBootAnimation(65, "Initializing Indicator Light");
        AC.indicatorLight.setup();

        ui.drawBootAnimation(70, "Initializing DFPlayer");
        assert(AC.player.setup() && "Player failed to initialize");

        ui.drawBootAnimation(75, "Loading sounds");
        AC.sounds = Sound::loadSounds(JSON_SOUNDS_FILE_NAME);

        ui.drawBootAnimation(85, "Alarms");
        setupAlarm(AC.alarm1);
        setupAlarm(AC.alarm2);
        setAlarm(AC.alarm1, AC.rtc);
        setAlarm(AC.alarm2, AC.rtc);

        ui.drawBootAnimation(90, "Setting UI Frames");
        ui.setFrames(
                UIDisplay::Frame{FRAME_CALLBACK(home), uiHome}, // 0
                UIDisplay::Frame{FRAME_CALLBACK(alarm), uiAlarm}, // 1
                UIDisplay::Frame{FRAME_CALLBACK(snoozeAlarm), uiAlarmSnooze}, // 2
                UIDisplay::Frame{FRAME_CALLBACK(defuseAlarm), uiAlarmDefuse}, // 3
                UIDisplay::Frame{FRAME_CALLBACK(overview), uiOverview}, // 4
                UIDisplay::Frame{FRAME_CALLBACK(settings), uiSettings}, // 5
                UIDisplay::Frame{FRAME_CALLBACK(alarmMenu), uiAlarmMenu}, // 6
                UIDisplay::Frame{FRAME_CALLBACK(alarmTime), uiAlarmTime}, // 7
                UIDisplay::Frame{FRAME_CALLBACK(alarmSound), uiAlarmSound}, // 8
                UIDisplay::Frame{FRAME_CALLBACK(playerMenu), uiPlayerMenu}, // 9
                UIDisplay::Frame{FRAME_CALLBACK(playerVolume), uiPlayerVolume}, // 10
                UIDisplay::Frame{FRAME_CALLBACK(playerPlay), uiPlayerPlay}, // 11
                UIDisplay::Frame{FRAME_CALLBACK(playerSounds), uiPlayerSounds}, // 12
                UIDisplay::Frame{FRAME_CALLBACK(lightDuration), uiLightDuration}, // 13
                UIDisplay::Frame{FRAME_CALLBACK(wifiMenu), uiWiFiMenu}, // 14
                UIDisplay::Frame{FRAME_CALLBACK(smartConfig), uiSmartConfig}, // 15
                UIDisplay::Frame{FRAME_CALLBACK(info), uiInfo} // 16
        );

        ui.drawBootAnimation(95, "Boot finished");
        ui.drawBootAnimation(100, "Boot finished");
    }

    /**
     * The main loop of the alarm clock
     */
    void loop() {
        static auto &matrix = AC.matrix;
        static auto &lightLevel = AC.lightLevel;
        static bool matrixIlluminate{false};
        static const ESP32_Timer matrixIlluminateTimer{
                "Matrix Illuminate Timer",
                5000,
                false,
                []() { matrixIlluminate = false; }
        };
        // callback to illuminate the matrix
        static const auto illuminateMatrix = []() {
            matrixIlluminate = true;
            matrixIlluminateTimer.start();
        };

        // alarm handle
        handleAlarms();

        matrix.loop();

        if (lightLevel < 1e-3 && !matrixIlluminate && navigation::read() != navigation::Direction::None) {
            illuminateMatrix();
        } else {
            // if the ui is animating, don't do anything else
            if (!AC.ui.loop()) return;
        }

        // handle light sensor and matrix illumination
        if (AC.lightSensor.tryReading()) {
            auto value = AC.lightSensor.getValue();
            if (lightLevel > 1e-3 && value < 1e-3) illuminateMatrix();
            lightLevel = value;
            matrix.setBrightness((uint8_t) (0.1005 * lightLevel - 0.05));
            matrix.shutdown(!AC.uiActive && lightLevel == 0 && !matrixIlluminate && !AC.mainLight.getDuty());
        }
    }

}


#endif //ALARM_CLOCK_MAIN_H
