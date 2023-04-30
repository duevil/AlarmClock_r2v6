//
// Created by Malte on 29.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_NAVIGATION_HPP
#define ALARM_CLOCK_R2V6_NAVIGATION_HPP

#include <utility>

#include "esp32/Touchpad.hpp"


/**
 * @brief Class to handle the navigation of the AlarmClock.
 * The navigation is done via the touchpad.
 * The touchpad is divided into 5 areas:\n
 * - MID\n
 * - LEFT\n
 * - RIGHT\n
 * - UP\n
 * - DOWN\n
 *
 * When a pad is touched, a callback is called with the pad that was touched.
 */
class Navigation {

public:
    enum class PadType : uint8_t {
        MID = 12,
        LEFT = 14,
        RIGHT = 27,
        UP = 32,
        DOWN = 33,
    };

private:
    Touchpad mid{static_cast<uint8_t>(PadType::MID)};
    Touchpad left{static_cast<uint8_t>(PadType::LEFT)};
    Touchpad right{static_cast<uint8_t>(PadType::RIGHT)};
    Touchpad up{static_cast<uint8_t>(PadType::UP)};
    Touchpad down{static_cast<uint8_t>(PadType::DOWN)};
    const std::array<std::pair<PadType, Touchpad *>, 5> pads{{
        {PadType::MID, &mid},
        {PadType::LEFT, &left},
        {PadType::RIGHT, &right},
        {PadType::UP, &up},
        {PadType::DOWN, &down},
    }};

    using callback_t = std::function<void(PadType)>;
    callback_t callback;

public:
    /**
     * @brief Creates a new Navigation object.
     * @param callback the callback that is called when a pad is touched
     */
    explicit Navigation(callback_t callback) : callback{std::move(callback)} {}

    /**
     * @brief Sets up all five touchpads.
     */
    void setup() const { for (auto &pair: pads) pair.second->setup(); }

    /**
     * @brief Checks if one of the touchpads is touched.
     * If one of the touchpads was touched, the callback is called once.
     */
    void loop() const {
        static bool touched{false};
        const PadType *pad{nullptr};

        for (auto &pair: pads) {
            if (pair.second->isTouched()) {
                pad = &pair.first;
                break;
            }
        }

        if (!touched && pad) {
            touched = true;
            DEBUG_F("pad=%d touched, calling callback", static_cast<int>(*pad));
            callback(*pad);
        } else if (touched && !pad) touched = false;
    }

    // delete copy constructor and assignment operator, making the class non-copyable

    Navigation(const Navigation &other) = delete;
    Navigation &operator=(const Navigation &other) = delete;

};


#endif //ALARM_CLOCK_R2V6_NAVIGATION_HPP
