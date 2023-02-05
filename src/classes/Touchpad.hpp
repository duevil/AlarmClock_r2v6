//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_H
#define ALARM_CLOCK_R2V6_TOUCHPAD_H

#include <Arduino.h>
#include <array>
#include <algorithm>
#include "functions/util.h"


class Touchpad {

    static constexpr uint8_t NUM_READINGS{5};
    static constexpr double PRECISION{0.8};
    static std::array<Touchpad, 5> pads;

    const uint8_t pin;
    std::array<uint16_t, NUM_READINGS> readings{};
    uint16_t value{};

    explicit Touchpad(uint8_t pin) : pin(pin) {};
    void init0();
    bool read0();

public:

    static Touchpad MID;
    static Touchpad LEFT;
    static Touchpad RIGHT;
    static Touchpad UP;
    static Touchpad DOWN;

    static void init();
    static Touchpad *read();

    inline bool operator==(Touchpad pad) const { return pin == pad.pin; }

    inline bool operator==(Touchpad const *pad) const { return pad && pin == pad->pin; }

};

/* DEFINITIONS */

Touchpad Touchpad::MID{12};
Touchpad Touchpad::LEFT{14};
Touchpad Touchpad::RIGHT{27};
Touchpad Touchpad::UP{32};
Touchpad Touchpad::DOWN{33};
std::array<Touchpad, 5> Touchpad::pads = {MID, LEFT, RIGHT, UP, DOWN};

void Touchpad::init() { for (auto &pad: pads) pad.init0(); }

Touchpad *Touchpad::read() {
    static bool isPressed = false;
    Touchpad *pressedPad = nullptr;

    for (auto &pad: pads) {
        if (pad.read0()) {
            pressedPad = &pad;
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

void Touchpad::init0() {
    uint16_t readValue;
    while ((readValue = touchRead(pin)) == 0);
    readings.fill(readValue);
    value = util::average<uint16_t, NUM_READINGS>(readings);
}

bool Touchpad::read0() {
    for (int i = 0; i < NUM_READINGS; ++i) {
        uint16_t readValue;
        while ((readValue = touchRead(pin)) == 0);
        readings[i] = readValue;
    }

    auto averageReading = util::average<uint16_t, NUM_READINGS>(readings);
    if (averageReading < (uint16_t) ((double) value * PRECISION)) return true;
    value = averageReading;
    return false;
}


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_H
