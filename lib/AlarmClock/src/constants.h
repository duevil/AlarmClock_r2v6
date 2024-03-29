//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_CONSTANTS_H
#define ALARM_CLOCK_CONSTANTS_H

constexpr auto RTC_SQW_PIN = 34;
constexpr auto LIGHT_PIN = 25;
constexpr auto INDICATOR_LED_PIN = 13;
constexpr auto I2C_SCL_PIN = 22;
constexpr auto I2C_SDA_PIN = 21;
constexpr auto SPI_CS_PIN = 5;
constexpr auto TOUCHPAD_CENTER_PIN = 12;
constexpr auto TOUCHPAD_LEFT_PIN = 14;
constexpr auto TOUCHPAD_RIGHT_PIN = 27;
constexpr auto TOUCHPAD_UP_PIN = 32;
constexpr auto TOUCHPAD_DOWN_PIN = 33;
constexpr auto SERVER_PORT = 8181;
constexpr auto JSON_SOUNDS_BUF_SIZE = 8192;
constexpr auto OLED_ADDRESS = 0x3C;
constexpr auto JSON_SOUNDS_FILE_NAME = "/sounds.json";
constexpr auto NTP_SERVER_1 = "pool.ntp.org";
constexpr auto NTP_SERVER_2 = "time.nist.gov";
constexpr auto NTP_SERVER_3 = "time.google.com";
constexpr auto PREFERENCES_NAMESPACE = "AlarmClock";

#endif //ALARM_CLOCK_CONSTANTS_H
