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
    enum class pad_t : uint8_t {
        MID = 12,
        LEFT = 14,
        RIGHT = 27,
        UP = 32,
        DOWN = 33,
    };

private:
    Touchpad mid{static_cast<uint8_t>(pad_t::MID)};
    Touchpad left{static_cast<uint8_t>(pad_t::LEFT)};
    Touchpad right{static_cast<uint8_t>(pad_t::RIGHT)};
    Touchpad up{static_cast<uint8_t>(pad_t::UP)};
    Touchpad down{static_cast<uint8_t>(pad_t::DOWN)};
    const std::array<std::pair<pad_t, Touchpad *>, 5> pads{{
        {pad_t::MID, &mid},
        {pad_t::LEFT, &left},
        {pad_t::RIGHT, &right},
        {pad_t::UP, &up},
        {pad_t::DOWN, &down},
    }};

    using callback_t = std::function<void(pad_t)>;
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
     * If one of the touchpads is touched, the callback is called once.
     */
    void loop() const {
        static bool touched{false};
        const pad_t *pad{nullptr};

        for (auto &pair: pads) {
            if (pair.second->isTouched()) {
                pad = &pair.first;
                break;
            }
        }

        if (!touched && pad) {
            touched = true;
            DEBUG_F("Touchpad::loop() pad=%d touched, calling callback", static_cast<int>(*pad));
            callback(*pad);
        } else if (touched && !pad) touched = false;
    }

    // delete copy constructor and assignment operator, making the class non-copyable

    Navigation(const Navigation &other) = delete;
    Navigation &operator=(const Navigation &other) = delete;

};


#endif //ALARM_CLOCK_R2V6_NAVIGATION_HPP
