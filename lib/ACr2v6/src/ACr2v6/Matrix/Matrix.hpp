//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MATRIX_H
#define ALARM_CLOCK_R2V6_MATRIX_H

#include <MD_Parola.h>
#include <functional>
#include "font.h"


namespace matrix {

    class Matrix {

    public:

        static void init(
                Property<float> &sensorProperty,
                Property<DateTime> &timeProperty,
                Property<uint8_t> &ledcProperty
        ) {
            static bool _init{false};
            assert(!_init);

            DEBUG_SIMPLE("Matrix initialization start");

            static MD_Parola md{MD_MAX72XX::FC16_HW, acc::MATRIX_CS_PIN, acc::MATRIX_NUM_DEVICES};
            static uint8_t intensity{0};
            static auto callback = [&ledcProperty]() {
                wasManuallyIlluminated = false;
                if (!intensity && !ledcProperty.get()) illumination.set(false);
            };

            md.begin();
            md.setIntensity(0);
            md.setFont(font);
            md.setCharSpacing(0);
            md.displayReset();
            md.displayClear();

            timeProperty.addListener([](const DateTime &, const DateTime &now) {
                char text[]{"hh:mm ss"};
                now.toString(text);
                text[6] = (char) (192 + text[6] - '0');
                text[7] = (char) (192 + text[7] - '0');
                md.displayText(text, PA_CENTER, 0, 0, PA_NO_EFFECT);
                md.displayAnimate();
            });

            sensorProperty.addListener([&ledcProperty](const float, const float lum) {
                intensity = getIntensity(lum);
                if (!wasManuallyIlluminated && (!ledcProperty.get() || intensity)) {
                    md.setIntensity(intensity);
                    illumination.set(intensity);
                }
            });

            ledcProperty.addListener([](uint8_t, uint8_t ledcVal) {
                if (ledcVal) illumination.set(true);
                else if (!intensity) illuminate();
            });

            illumination.addListener([&ledcProperty](const bool, const bool isIlluminated) {
                DEBUG("Matrix illumination: ", isIlluminated);
                if (!intensity && (ledcProperty.get() || wasManuallyIlluminated)) md.displayShutdown(false);
                else md.displayShutdown(!isIlluminated);
            });

            timerCallback = [](TimerHandle_t) { callback(); };
            illumination.set(true);

            _init = true;
            DEBUG_SIMPLE("Matrix initialization end");
        }

        static void illuminate() {
            static const auto tmr = xTimerCreate(
                    "matrix manual illumination",
                    acc::MATRIX_ILLUMINATION_DURATION,
                    pdFALSE,
                    nullptr,
                    timerCallback
            );

            if (!wasManuallyIlluminated) {
                wasManuallyIlluminated = true;
                illumination.set(true);
            }
            DEBUG("xTimer started: ", pcTimerGetTimerName(tmr));
            xTimerReset(tmr, 0);
        }

        inline static bool isIlluminated() { return illumination.get(); }

    private:

        static Property<bool> illumination;
        static bool wasManuallyIlluminated;
        static TimerCallbackFunction_t timerCallback;

        static uint8_t getIntensity(const float lum) { return (uint8_t) (lum * acc::MATRIX_LIGHT_ADJUSTMENT); }

    };

    Property<bool> Matrix::illumination{};
    bool Matrix::wasManuallyIlluminated{false};
    TimerCallbackFunction_t Matrix::timerCallback{};

}

using matrix::Matrix;


#endif //ALARM_CLOCK_R2V6_MATRIX_H
