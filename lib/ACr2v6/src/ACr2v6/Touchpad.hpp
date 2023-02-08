//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_H
#define ALARM_CLOCK_R2V6_TOUCHPAD_H

#include <array>
#include <algorithm>


/**
 * @brief Implements the reading off the AlarmClock's five different touchpads
 */
class Touchpad {

public:

    static Touchpad MID;
    static Touchpad LEFT;
    static Touchpad RIGHT;
    static Touchpad UP;
    static Touchpad DOWN;

    static void init() {
        assert(!_init);
        for (auto &pad: pads) pad->init0();
        _init = true;
    }

    static Touchpad *read() {
        assert(_init);
        static bool isPressed = false;
        Touchpad *pressedPad = nullptr;
        for (auto &pad: pads) {
            if (pad->read0()) {
                pressedPad = pad;
                break;
            }
        }
        if (!isPressed && pressedPad != nullptr) {
            isPressed = true;
            return pressedPad;
        }
        if (isPressed && pressedPad == nullptr) isPressed = false;

        return nullptr;
    }

    inline bool operator==(Touchpad const *pad) const { return pad && pin == pad->pin; }

    const char* toString() const {
        if (MID == this) return "MID";
        if (LEFT == this) return "LEFT";
        if (RIGHT == this) return "RIGHT";
        if (UP == this) return "UP";
        if (DOWN == this) return "DOWN";
    }

private:

    static constexpr uint8_t NUM_READINGS = 5;
    static constexpr double PRECISION = 0.65;
    static const std::array<Touchpad*, 5> pads;
    static bool _init;

    const uint8_t pin;
    std::array<uint16_t, NUM_READINGS> readings{};
    uint16_t thresholdValue{};

    explicit Touchpad(uint8_t pin) : pin(pin) {};

    void init0() {
        assert(!_init);

        uint16_t readValue;
        for (int i = 0; i < NUM_READINGS; ++i) {
            while ((readValue = touchRead(pin)) == 0);
            readings[i] = readValue;
        }
        thresholdValue = util::average<uint16_t, NUM_READINGS>(readings);

        DEBUG("Touchpad initialized: ", pin);
        DEBUG("Threshold value: ", thresholdValue);
    }

    bool read0() {
        assert(_init);

        uint16_t readValue;
        for (int i = 0; i < NUM_READINGS; ++i) {
            while ((readValue = touchRead(pin)) == 0);
            readings[i] = readValue;
        }

        readValue = util::average<uint16_t, NUM_READINGS>(readings);
        if (readValue < (uint16_t) ((double) thresholdValue * PRECISION)) return true;

        thresholdValue = readValue;
        return false;
    }

};

Touchpad Touchpad::MID{12};
Touchpad Touchpad::LEFT{14};
Touchpad Touchpad::RIGHT{27};
Touchpad Touchpad::UP{32};
Touchpad Touchpad::DOWN{33};
bool Touchpad::_init = false;
const std::array<Touchpad*, 5> Touchpad::pads{&MID, &LEFT, &RIGHT, &UP, &DOWN};


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_H
