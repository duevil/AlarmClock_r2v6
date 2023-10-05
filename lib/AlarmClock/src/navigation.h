//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_NAVIGATION_H
#define ALARM_CLOCK_NAVIGATION_H


namespace AlarmClock {
    namespace navigation {

        /**
         * Enum class for the navigation direction
         */
        enum class Direction {
            Center,
            Left,
            Right,
            Up,
            Down,
            None
        };

        /**
         * Nav pad struct containing the direction and the touchpad
         */
        struct Pad {
            const Direction direction;
            std::unique_ptr<Touchpad> touchpad;

            Pad(Direction dir, uint8_t pin) : direction{dir}, touchpad{new Touchpad{pin}} {}
        };

        /**
         * The nav pads
         */
        const std::array<Pad, 5> navPads{
                Pad{Direction::Center, TOUCHPAD_CENTER_PIN},
                Pad{Direction::Left, TOUCHPAD_LEFT_PIN},
                Pad{Direction::Right, TOUCHPAD_RIGHT_PIN},
                Pad{Direction::Up, TOUCHPAD_UP_PIN},
                Pad{Direction::Down, TOUCHPAD_DOWN_PIN}
        };


        /**
         * Sets up the navigation touch pads
         */
        void setup() { for (const auto &navPad: navPads) navPad.touchpad->setup(); }

        /**
         * Reads the navigation touch pads and returns the first direction that was touched\n
         * To ensure proper functionality, this function should be called in every loop iteration.
         * Returns the direction only once after initial touch.
         * @return The direction that was touched or Direction::None if no direction was touched
         */
        Direction read() {
            static bool touched{false};
            Direction dir{Direction::None};

            for (const auto &navPad: navPads) {
                if (navPad.touchpad->isTouched()) {
                    dir = navPad.direction;
                    break;
                }
            }

            if (!touched && dir != Direction::None) {
                touched = true;
                return dir;
            }
            if (touched && dir == Direction::None) {
                touched = false;
            }

            return Direction::None;
        }

    }
}


#endif //ALARM_CLOCK_NAVIGATION_H
