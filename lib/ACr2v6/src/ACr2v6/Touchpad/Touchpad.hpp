//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_TOUCHPAD_HPP
#define ALARM_CLOCK_R2V6_TOUCHPAD_HPP


namespace touchpad {

    /**
     * @brief Implements the reading off the AlarmClock's touch capacitors
     */
    class Touchpad final : private Initialisable {

    public:

        /**
         * @param pad The pad associated to this Touchpad and defining this Touchpad's physical pin
         */
        explicit Touchpad(pad_t pad) : pad(pad), pin(static_cast<const uint8_t>(pad)) { assert(pin > 0); };
        Touchpad(Touchpad &) = delete;
        Touchpad &operator=(const Touchpad &) = delete;

        /**
         * @brief Initializes this Touchpad; calculates the threshold value used when reading the Touchpad 
         */
        void init() override {
            DEBUG("Initializing touchpad: ", pin);
            setInit();

            thresholdValue = touchRead();

            DEBUG("Touchpad initialized: ", pin);
            DEBUG("Threshold value: ", thresholdValue);
        }

        /**
         * @brief Read the Touchpad's capacitor's value and checks whether this value is below the threshold;
         * if this is not the case, the read value will be used as the next threshold
         * @return true, when the read value is below the current threshold
         */
        bool read() {
            assert(isInit());

            auto readValue = touchRead();
            if (readValue < (uint16_t) ((float) thresholdValue * precision)) return true;
            thresholdValue = readValue;
            return false;
        }

        /**
         * @return The pad enum value for this Touchpad
         */
        inline const pad_t &getPad() const { return pad; }

    private:

        uint16_t touchRead() {
            for (int i{0}; i < readingsAmount; ++i) while ((readings[i] = ::touchRead(pin)) == 0);
            return util::average<uint16_t, readingsAmount>(readings);
        }

        constexpr static uint8_t readingsAmount{acc::TOUCHPAD_READINGS};
        constexpr static float_t precision{acc::TOUCHPAD_PRECISION};
        const pad_t pad;
        const uint8_t pin;
        std::array<uint16_t, readingsAmount> readings{};
        uint16_t thresholdValue{};

    };

}


#endif //ALARM_CLOCK_R2V6_TOUCHPAD_H
