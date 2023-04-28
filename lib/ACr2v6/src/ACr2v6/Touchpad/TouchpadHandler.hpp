//
// Created by Malte on 08.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_HANDLER_HPP
#define ALARM_CLOCK_R2V6_TOUCHPAD_HANDLER_HPP


namespace touchpad {

    /**
     * A handler for the Alarm Clock's Touchpads
     */
    class TouchpadHandler final : private Initialisable {

    public:

        using PressedListener = std::function<void(pad_t)>;

        /**
         * @param pressedListener The listener to be called when a Touchpad was pressed
         */
        explicit TouchpadHandler(const PressedListener &pressedListener) : pressedListener(pressedListener) {}

        /**
         * @brief Initializes the handler's Touchpads
         */
        void init() override {
            DEBUG_SIMPLE("Touchpad initialization start");
            setInit();

            for (auto &p: pads) p->init();

            DEBUG_SIMPLE("Touchpad initialization end");
        }

        /**
         * @brief Reads the handler's Touchpads and calls the pressed listener if a pad was pressed
         */
        void run() const {
            assert(isInit());

            static bool isPressed{false};
            const pad_t *pressedPad{nullptr};

            for (auto &p: pads) {
                if (p->read()) {
                    pressedPad = &p->getPad();
                    break;
                }
            }

            if (!isPressed && pressedPad) {
                isPressed = true;
                pressedListener(*pressedPad);
            } else if (isPressed && !pressedPad) isPressed = false;
        }

    private:

        const PressedListener &pressedListener;
        const pad_vector_t &pads{touchpads_from_enum()};

    };

}


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_HANDLER_HPP
