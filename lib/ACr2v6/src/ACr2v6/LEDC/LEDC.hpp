//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MAIN_LIGHT_H
#define ALARM_CLOCK_R2V6_MAIN_LIGHT_H

#include "LightProperty.hpp"
#include "DurationProperty.hpp"


namespace ac_r2v6 {

    /**
     * @brief Controller for the AlarmClock's main LEDC lights
     */
    class LEDC final : private Initialisable {

    public:

        /**
         * @brief Initializes the LEDC channel and property listener
         */
        void init() override {
            setInit();

            static auto turnLightOff = [this]() { value.turnOff(); };
            static auto timer = xTimerCreate(
                    "light timer",
                    calcLightTicks(0),
                    pdFALSE,
                    nullptr,
                    [](TimerHandle_t) { turnLightOff(); }
            );

            ledcSetup(acc::LIGHT_CHANNEL, acc::LIGHT_FREQUENCY, acc::LIGHT_RESOLUTION);
            ledcAttachPin(acc::LIGHT_PIN, acc::LIGHT_CHANNEL);

            value.addListener([](uint8_t, uint8_t d) {
                ledcWrite(acc::LIGHT_CHANNEL, acc::LIGHT_DUTIES[d]);
                if (d) {
                    DEBUG("xTimer started: ", pcTimerGetTimerName(timer));
                    xTimerReset(timer, 0);
                }
            });
            duration.addListener([](uint8_t, uint8_t d) {
                auto state{xTimerIsTimerActive(timer)};
                xTimerChangePeriod(timer, calcLightTicks(d), 0);
                if (!state) xTimerStop(timer, 0);
            });
        }

        /**
         * @return The property containing the LEDC value
         */
        inline LightProperty &valueProperty() { return value; }

        /**
         * @return The property containing the duration for the LEDC before turning off
         */
        inline DurationProperty &durationProperty() { return duration; }

    private:

        LightProperty value{};
        DurationProperty duration{};

        /**
         * @brief Calculates the duration in ticks the light will remain on
         * @param d The duration value used for the calculation
         * @return The calculated value in timer ticks
         */
        static constexpr uint32_t calcLightTicks(const uint8_t d) {
            return (acc::LIGHT_DURATIONS[d] * 60 * 1000) / portTICK_PERIOD_MS;
        }

    };
}


#endif //ALARM_CLOCK_R2V6_MAIN_LIGHT_H
