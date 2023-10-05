//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_ESP32_WIFI_H
#define ALARM_CLOCK_ESP32_WIFI_H


namespace AlarmClock {
    namespace esp32_wifi {

        bool smartConfig() {
            WiFiClass::mode(WIFI_AP_STA);
            auto result = WiFi.beginSmartConfig();
            assert(result && "WiFi failed to start SmartConfig");
            auto i = 0;
            while (!WiFi.smartConfigDone()) {
                delay(500);
                if (i++ > 30) { // 15 seconds
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Check if the device is connected to WiFi
         * @return true if connected, false otherwise
         */
        bool isConnected() { return WiFi.isConnected(); }

        bool waitForConnection() {
            for (int i = 0; i < 10; i++) {
                if (isConnected()) return true;
                delay(500);
            }
            return false;
        }

        /**
         * @brief Connect to WiFi and start a timer that checks the connection every 30 minutes\n
         * Tries to connect to the last known WiFi network.
         * If that fails, it starts SmartConfig and tries to connect again.
         * @return true if WiFi was connected successfully, false otherwise
         */
        template<typename SmartConfigStartCallback>
        bool setup(SmartConfigStartCallback smartConfigStartCallback) {
            static ESP32_Timer connectionTimer{
                    "WiFi Connection Timer",
                    1800000 /* 30 minutes */,
                    true,
                    []() {
                        if (!isConnected()) {
                            WiFi.begin();
                            waitForConnection();
                        }
                    }
            };
            connectionTimer.start();
            WiFi.begin();
            if (waitForConnection()) {
                WiFiClass::setHostname("AlarmClock");
                return true;
            } else {
                smartConfigStartCallback();
                if (smartConfig()) {
                    WiFi.begin();
                    if (waitForConnection()) {
                        WiFiClass::setHostname("AlarmClock");
                        return true;
                    }
                }
            }
            return false;
        }

        /**
         * @brief Disable WiFi
         * @return true if WiFi was disconnected and disabled successfully, false otherwise
         */
        bool disable() { return WiFi.disconnect(true) && WiFiClass::mode(WIFI_OFF); }

        /**
         * @brief Setup NTP
         * @tparam NTPCallback The callback type for the NTP sync notification
         * @param ntpCallback The callback function for the NTP sync notification
         * @param tz The timezone to use
         */
        template<typename NTPCallback>
        void setupNTP(NTPCallback ntpCallback, const char* tz) {
            sntp_set_time_sync_notification_cb(ntpCallback);
            configTzTime(tz, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
        }
    }
}


#endif //ALARM_CLOCK_ESP32_WIFI_H
