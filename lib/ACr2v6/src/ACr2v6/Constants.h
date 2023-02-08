//
// Created by Malte on 08.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_CONSTANTS_H
#define ALARM_CLOCK_R2V6_CONSTANTS_H


/**
 * @brief Namespace containing all constant expressions for the alarm clock's elements
 */
namespace acc {

    // storage
    constexpr const char *const PREFERENCES_NAME = "ACr2v2";
    // rtc
    constexpr uint8_t RTC_INTERRUPT_PIN = 34;
    // touchpad
    constexpr uint8_t TOUCHPAD_PIN_MID = 12;
    constexpr uint8_t TOUCHPAD_PIN_LEFT = 14;
    constexpr uint8_t TOUCHPAD_PIN_RIGHT = 27;
    constexpr uint8_t TOUCHPAD_PIN_UP = 32;
    constexpr uint8_t TOUCHPAD_PIN_DOWN = 33;
    constexpr uint8_t TOUCHPAD_READINGS = 5;
    constexpr float_t TOUCHPAD_PRECISION = 0.65f;
    // matrix
    constexpr uint8_t MATRIX_CS_PIN = 5;
    constexpr uint8_t MATRIX_NUM_DEVICES = 4;
    constexpr float_t MATRIX_LIGHT_ADJUSTMENT = 0.2f;
    constexpr uint16_t MATRIX_ILLUMINATION_DURATION = 2000;
    // light sensor
    constexpr uint8_t LIGHT_SENSOR_READINGS = 15;
    constexpr uint8_t LIGHT_SENSOR_ADJUSTMENT = 75;

} // alarm clock constants

#endif //ALARM_CLOCK_R2V6_CONSTANTS_H
