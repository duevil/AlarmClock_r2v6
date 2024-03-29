//
// Created by Malte on 02.10.2023.
//

#ifndef ALARM_CLOCK_H
#define ALARM_CLOCK_H

// external third party libraries
#include <array>
#include <utility>
#include <vector>
#include <memory>
#include <esp_sntp.h>
#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPIFFS.h>
#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "AsyncElegantOTA.h"
#include "DFRobotDFPlayerMini.h"
// external first party libraries
#include "Matrix32x8.h"
#include "BH1750_LightSensor.h"
#include "ESP32_Touchpad.h"
#include "ESP32_SimpleLEDC.h"
#include "ESP32_Timer.h"
#include "UserInterface.h"
// internal classes and headers
#include "constants.h"
#include "Bean.hpp"
#include "MainLight.hpp"
#include "Sound.hpp"
#include "Player.hpp"
#include "alarm.h"
#include "AlarmClock.hpp"
// internal functions
#include "bitmaps.h"
#include "navigation.h"
#include "rtc_ds3231.h"
#include "esp32_wifi.h"
#include "webserver.h"
#include "alarm_handler.h"
#include "ui_frames.h"
#include "ui_handles.h"
#include "ac_main.h" // needs to be included last

#endif //ALARM_CLOCK_H
