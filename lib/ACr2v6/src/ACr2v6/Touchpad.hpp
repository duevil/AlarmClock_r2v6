//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_H
#define ALARM_CLOCK_R2V6_TOUCHPAD_H

#include <array>
#include <vector>
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

    Touchpad(const Touchpad &) = delete;
    Touchpad& operator=(const Touchpad &) = delete;

    inline bool operator==(Touchpad const *pad) const { return pad && pin == pad->pin; }

    inline bool operator!=(Touchpad const *pad) const { return (*this == pad); }

    const char *toString() const {
        if (MID == this) return __STRING(MID);
        if (LEFT == this) return __STRING(LEFT);
        if (RIGHT == this) return __STRING(RIGHT);
        if (UP == this) return __STRING(UP);
        if (DOWN == this) return __STRING(DOWN);
        return "THIS MUST NOT BE REACHED";
    }

private:

    explicit Touchpad(uint8_t pin) : pin(pin) {};

    static const std::vector<Touchpad *> pads;
    static bool _init;

    const uint8_t pin;
    std::array<uint16_t, acc::TOUCHPAD_READINGS> readings{};
    uint16_t thresholdValue{};

    void init0() {
        assert(!_init);

        uint16_t readValue;
        for (int i = 0; i < acc::TOUCHPAD_READINGS; ++i) {
            while ((readValue = touchRead(pin)) == 0);
            readings[i] = readValue;
        }
        thresholdValue = util::average<uint16_t, acc::TOUCHPAD_READINGS>(readings);

        DEBUG("Touchpad initialized: ", pin);
        DEBUG("Threshold value: ", thresholdValue);
    }

    bool read0() {
        assert(_init);

        uint16_t readValue;
        for (int i = 0; i < acc::TOUCHPAD_READINGS; ++i) {
            while ((readValue = touchRead(pin)) == 0);
            readings[i] = readValue;
        }

        readValue = util::average<uint16_t, acc::TOUCHPAD_READINGS>(readings);
        if (readValue < (uint16_t) ((float) thresholdValue * acc::TOUCHPAD_PRECISION)) return true;

        thresholdValue = readValue;
        return false;
    }

};

Touchpad Touchpad::MID{acc::TOUCHPAD_PIN_MID};
Touchpad Touchpad::LEFT{acc::TOUCHPAD_PIN_LEFT};
Touchpad Touchpad::RIGHT{acc::TOUCHPAD_PIN_RIGHT};
Touchpad Touchpad::UP{acc::TOUCHPAD_PIN_UP};
Touchpad Touchpad::DOWN{acc::TOUCHPAD_PIN_DOWN};
bool Touchpad::_init = false;
const std::vector<Touchpad *> Touchpad::pads{&MID, &LEFT, &RIGHT, &UP, &DOWN};


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_H
