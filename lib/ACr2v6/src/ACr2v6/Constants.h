//
// Created by Malte on 08.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_CONSTANTS_H
#define ALARM_CLOCK_R2V6_CONSTANTS_H


/**
 * @brief Namespace containing all constant expressions for the alarm clock's elements
 */
namespace acc {

    // Alarm
    constexpr uint32_t ALARM_MISSED_TIMER{(10 * 60 * 1000) / portTICK_PERIOD_MS};
    // storage
    constexpr const char *const PREFERENCES_NAME{"ACr2v2"};
    // rtc
    constexpr uint8_t RTC_INTERRUPT_PIN{34};
    // touchpad
    constexpr uint8_t TOUCHPAD_PIN_MID{12};
    constexpr uint8_t TOUCHPAD_PIN_LEFT{14};
    constexpr uint8_t TOUCHPAD_PIN_RIGHT{27};
    constexpr uint8_t TOUCHPAD_PIN_UP{32};
    constexpr uint8_t TOUCHPAD_PIN_DOWN{33};
    constexpr uint8_t TOUCHPAD_READINGS{5};
    constexpr float_t TOUCHPAD_PRECISION{0.50f};
    // matrix
    constexpr uint8_t MATRIX_CS_PIN{5};
    constexpr uint8_t MATRIX_NUM_DEVICES{4};
    constexpr float_t MATRIX_LIGHT_ADJUSTMENT{0.2f};
    constexpr uint16_t MATRIX_ILLUMINATION_DURATION{2000 / portTICK_PERIOD_MS};
    // light sensor
    constexpr uint8_t LIGHT_SENSOR_READINGS{15};
    constexpr uint8_t LIGHT_SENSOR_ADJUSTMENT{75};
    // light ledc
    constexpr uint8_t LIGHT_CHANNEL{0};
    constexpr uint16_t LIGHT_FREQUENCY{5000};
    constexpr uint8_t LIGHT_RESOLUTION{8};
    constexpr uint8_t LIGHT_PIN{25};
    constexpr uint8_t LIGHT_DUTIES[]{
            0, 16, 64, 255
    }; // the possible light duties as a logarithmic scale
    constexpr uint8_t LIGHT_DUTIES_SIZE{sizeof(LIGHT_DUTIES) / sizeof(LIGHT_DUTIES[0])};
    constexpr uint8_t LIGHT_DURATIONS[]{
            5, 10, 20, 30, 45, 60, 90, 120, 180, 240
    }; // the possible durations in minutes
    constexpr uint8_t LIGHT_DURATIONS_SIZE{sizeof(LIGHT_DURATIONS) / sizeof(LIGHT_DURATIONS[0])};

} // alarm clock constants

#endif //ALARM_CLOCK_R2V6_CONSTANTS_H
