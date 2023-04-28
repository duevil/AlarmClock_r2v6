//
// Created by Malte on 09.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_ENUM_H
#define ALARM_CLOCK_R2V6_TOUCHPAD_ENUM_H

namespace touchpad {

    /**
     * An enum for the Alarm Clock's five touchpad segments
     */
    enum class pad_t : int8_t {
        MID = acc::TOUCHPAD_PIN_MID,
        LEFT = acc::TOUCHPAD_PIN_LEFT,
        RIGHT = acc::TOUCHPAD_PIN_RIGHT,
        UP = acc::TOUCHPAD_PIN_UP,
        DOWN = acc::TOUCHPAD_PIN_DOWN,
    };

}

#endif //ALARM_CLOCK_R2V6_TOUCHPAD_ENUM_H
