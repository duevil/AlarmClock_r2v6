//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MATRIX_H
#define ALARM_CLOCK_R2V6_MATRIX_H

#include <MD_Parola.h>
#include "font.h"


namespace matrix {

    /**
     * @brief Controls the Alarm Clock's matrix display
     */
    class Matrix final : private Initialisable {

    public:

        /**
         * @param sensorProperty The property wrapping the light sensor value
         * @param timeProperty The property wrapping the current time value
         * @param ledcProperty The property wrapping the current ledc value
         */
        Matrix(Property<float> &sensorProperty,
               Property<DateTime> &timeProperty,
               Property<uint8_t> &ledcProperty)
                : sensorProperty(sensorProperty),
                  timeProperty(timeProperty),
                  ledcProperty(ledcProperty) {}

        /**
         * @brief Initializes the matrix; initializes the hardware and links the property listeners
         */
        void init() override {
            DEBUG_SIMPLE("Matrix initialization start");
            setInit();

            static MD_Parola md{MD_MAX72XX::FC16_HW, acc::MATRIX_CS_PIN, acc::MATRIX_NUM_DEVICES};

            md.begin();
            md.setIntensity(0);
            md.setFont(font);
            md.setCharSpacing(0);
            md.displayReset();
            md.displayClear();

            timeProperty.addListener([](const DateTime &, const DateTime &now) {
                String text{"hh:mm ss"};
                now.toString(text.begin());
                text[6] = (char) (192 + text[6] - '0');
                text[7] = (char) (192 + text[7] - '0');
                md.displayText(text.c_str(), PA_CENTER, 0, 0, PA_NO_EFFECT);
                md.displayAnimate();
            });

            sensorProperty.addListener([this](const float, const float lum) {
                intensity = getIntensity(lum);
                if (!wasManuallyIlluminated && (!ledcProperty.get() || intensity)) {
                    md.setIntensity(intensity);
                    illumination.set(intensity);
                }
            });

            ledcProperty.addListener([this](uint8_t, uint8_t ledcVal) {
                if (ledcVal) illumination.set(true);
                else if (!intensity) illuminate();
            });

            illumination.addListener([this](const bool, const bool isIlluminated) {
                DEBUG("Matrix illumination: ", isIlluminated);
                if (!intensity && (ledcProperty.get() || wasManuallyIlluminated)) md.displayShutdown(false);
                else md.displayShutdown(!isIlluminated);
            });

            illumination.set(true);

            DEBUG_SIMPLE("Matrix initialization end");
        }

        /**
         * Manually illuminates the matrix if it was not illuminated by the light sensor's value
         * and starts the illumination timer
         */
        void illuminate() {
            assert(isInit());

            static auto callback = [this]() {
                wasManuallyIlluminated = false;
                if (!intensity && !ledcProperty.get()) illumination.set(false);
            };
            static const auto tmr = xTimerCreate(
                    "matrix manual illumination",
                    acc::MATRIX_ILLUMINATION_DURATION,
                    pdFALSE,
                    nullptr,
                    [](TimerHandle_t) { callback(); }
            );

            if (!wasManuallyIlluminated) {
                wasManuallyIlluminated = true;
                illumination.set(true);
            }
            DEBUG("xTimer started: ", pcTimerGetTimerName(tmr));
            xTimerReset(tmr, 0);
        }

        /**
         * @return true, when the matrix is illuminated
         */
        inline bool isIlluminated() const { return illumination.get(); }

    private:

        Property<float> &sensorProperty;
        Property<DateTime> &timeProperty;
        Property<uint8_t> &ledcProperty;
        Property<bool> illumination{};
        uint8_t intensity{0};
        bool wasManuallyIlluminated{false};

        /**
         * @brief Calculates the intensity for the given light value
         * @param lum The value of the light sensor
         * @return The calculated matrix led intensity (0-15)
         */
        static uint8_t getIntensity(const float lum) { return (uint8_t) (lum * acc::MATRIX_LIGHT_ADJUSTMENT); }

    };

}

namespace ac_r2v6 {
    using matrix::Matrix;
}


#endif //ALARM_CLOCK_R2V6_MATRIX_H
