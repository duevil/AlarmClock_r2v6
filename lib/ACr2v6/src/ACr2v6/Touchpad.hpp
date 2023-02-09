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

    enum class pad_t : int8_t {
        MID = acc::TOUCHPAD_PIN_MID,
        LEFT = acc::TOUCHPAD_PIN_LEFT,
        RIGHT = acc::TOUCHPAD_PIN_RIGHT,
        UP = acc::TOUCHPAD_PIN_UP,
        DOWN = acc::TOUCHPAD_PIN_DOWN,
        NONE = -1,
    };

    static void init() {
        assert(!_init);
        DEBUG_SIMPLE("Touchpad initialization start");

        for (Touchpad &p: pads) p.init0();

        _init = true;
        DEBUG_SIMPLE("Touchpad initialization end");
    }

    static pad_t read() {
        assert(_init);

        static bool isPressed{false};
        pad_t pressedPad{pad_t::NONE};
        for (Touchpad &p: pads) {
            if (p.read0()) {
                pressedPad = p.pad;
                break;
            }
        }
        if (!isPressed && pressedPad != pad_t::NONE) {
            isPressed = true;
            return pressedPad;
        }
        if (isPressed && pressedPad == pad_t::NONE) isPressed = false;

        return pad_t::NONE;
    }

    Touchpad(const Touchpad &) = delete;
    Touchpad &operator=(const Touchpad &) = delete;

#   ifndef NDEBUG

    static const char *toString(pad_t pad) {
        switch (pad) {
            case pad_t::MID:
                return "MID";
            case pad_t::LEFT:
                return "LEFT";
            case pad_t::RIGHT:
                return "RIGHT";
            case pad_t::UP:
                return "UP";
            case pad_t::DOWN:
                return "DOWN";
            case pad_t::NONE:
                return "NONE";
        }
    }

#   endif

private:

    explicit Touchpad(pad_t pad) : pad(pad), pin(static_cast<const uint8_t>(pad)) { assert(pin > 0); };

    static Touchpad P_MID;
    static Touchpad P_LEFT;
    static Touchpad P_RIGHT;
    static Touchpad P_UP;
    static Touchpad P_DOWN;
    static const std::vector<std::reference_wrapper<Touchpad>> pads;
    static bool _init;

    const pad_t pad;
    const uint8_t pin;
    std::array<uint16_t, acc::TOUCHPAD_READINGS> readings{};
    uint16_t thresholdValue{};

    void init0() {
        assert(!_init);

        uint16_t readValue;
        for (int i{0}; i < acc::TOUCHPAD_READINGS; ++i) {
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
        for (int i{0}; i < acc::TOUCHPAD_READINGS; ++i) {
            while ((readValue = touchRead(pin)) == 0);
            readings[i] = readValue;
        }

        readValue = util::average<uint16_t, acc::TOUCHPAD_READINGS>(readings);
        if (readValue < (uint16_t) ((float) thresholdValue * acc::TOUCHPAD_PRECISION)) return true;

        thresholdValue = readValue;
        return false;
    }

};

Touchpad Touchpad::P_MID{pad_t::MID};
Touchpad Touchpad::P_LEFT{pad_t::LEFT};
Touchpad Touchpad::P_RIGHT{pad_t::RIGHT};
Touchpad Touchpad::P_UP{pad_t::UP};
Touchpad Touchpad::P_DOWN{pad_t::DOWN};
const std::vector<std::reference_wrapper<Touchpad>> Touchpad::pads{P_MID, P_LEFT, P_RIGHT, P_UP, P_DOWN};
bool Touchpad::_init{false};


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_H
