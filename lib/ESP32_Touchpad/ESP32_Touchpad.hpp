//
// Created by Malte on 29.04.2023.
//

#ifndef ESP32_TOUCHPAD_HPP
#define ESP32_TOUCHPAD_HPP


/**
 * @brief A class for reading one of the touch-capacitive pins of the ESP32.\n
 * The pin is read multiple times to get a good average value.
 * The class also provides a method to check if the pin is touched.
 */
class Touchpad {

private:
    constexpr static auto READINGS = 10;
    constexpr static auto TOLERANCE = 0.9f;

    const uint8_t pin;
    AveragedValue<float, READINGS> value{};
    float threshold{};

     void touchRead() {
        float val;
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
        for (int i = 0; i < READINGS; ++i) touchRead();
        threshold = value * TOLERANCE;
    }

    /**
     * @brief Reads the touch-capacitive pin and checks if its value is below the threshold.
     * If the pin was not touched, the threshold is updated.
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


#endif //ESP32_TOUCHPAD_HPP
