//
// Created by Malte on 29.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_HPP
#define ALARM_CLOCK_R2V6_TOUCHPAD_HPP

#include "base/AveragedValue.hpp"


/**
 * @brief A class for reading one of the touch-capacitive pins of the ESP32.
 */
class Touchpad {

private:
    constexpr static int READINGS = 10;
    constexpr static float TOLERANCE = 0.9f;

    const uint8_t pin;
    AveragedValue<float, READINGS> value{};
    float threshold{};

    /**
     * @brief Reads the touch-capacitive pin and updates the average value.
     */
    void touchRead() {
        float val{};
        while ((val = ::touchRead(pin)) == 0);
        value = val;
    }

public:
    explicit Touchpad(uint8_t pin) : pin{pin} {}

    /**
     * @brief Sets up the touch-capacitive pin.
     * Reads the pin multiple times to get a good average value.
     * Sets the threshold to the average value times the tolerance.
     */
    void setup() {
        DEBUG_F("Touchpad::setup() pin=%d", pin);
        for (int i = 0; i < READINGS; ++i) touchRead();
        threshold = value * TOLERANCE;
        DEBUG_F("Touchpad::setup() threshold=%f", threshold);
    }

    /**
     * @brief Checks if the touch-capacitive pin is touched.
     * @return True if the pin is touched, false otherwise.
     */
    bool isTouched() {
        touchRead();
        if (value < threshold) return true;
        threshold = (threshold + value) / 2 * TOLERANCE;
        return false;
    }

    Touchpad(const Touchpad &other) = delete;
    Touchpad &operator=(const Touchpad &other) = delete;

};


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_HPP
