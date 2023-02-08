//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MATRIX_H
#define ALARM_CLOCK_R2V6_MATRIX_H

#include <MD_Parola.h>
#include "font.h"


namespace matrix {

    class Matrix {

    public:

        static void init(Property<float> &lightProperty, Property<DateTime> &timeProperty) {
            static bool _init = false;
            static MD_Parola md{MD_MAX72XX::FC16_HW, acc::MATRIX_CS_PIN, acc::MATRIX_NUM_DEVICES};
            static uint8_t intensity = 0;

            assert(!_init);

            DEBUG_SIMPLE("Matrix initialization start");

            md.begin();
            md.setIntensity(0);
            md.setFont(font);
            md.setCharSpacing(0);
            md.displayReset();
            md.displayClear();

            timeProperty.addListener([](const DateTime &, const DateTime &now) {
                char text[] = "hh:mm ss";
                now.toString(text);
                DEBUG("Matrix time: ", text);
                text[6] = (char) (192 + text[6] - '0');
                text[7] = (char) (192 + text[7] - '0');
                md.displayText(text, PA_CENTER, 0, 0, PA_NO_EFFECT);
                md.displayAnimate();
            });

            lightProperty.addListener([](const float, const float lum) {
                intensity = (uint8_t) (lum * acc::MATRIX_LIGHT_ADJUSTMENT);
                if (!wasManuallyIlluminated) {
                    illumination.set(intensity > 0);
                    md.setIntensity(intensity);
                }
            });

            illumination.addListener([](const bool, const bool isIlluminated) {
                DEBUG("Matrix illumination: ", isIlluminated);
                if (!isIlluminated) md.setIntensity(0);
                if (isIlluminated && wasManuallyIlluminated) md.setIntensity(1);
                md.displayShutdown(!(isIlluminated || wasManuallyIlluminated));
            });

            illumination.set(true);
            _init = true;
            DEBUG_SIMPLE("Matrix initialization end");
        }

        static void illuminate() {
            static const auto tmr = xTimerCreate(
                    "matrix manual illumination",
                    acc::MATRIX_ILLUMINATION_DURATION / portTICK_PERIOD_MS,
                    pdFALSE,
                    nullptr,
                    [](TimerHandle_t) {
                        wasManuallyIlluminated = false;
                        illumination.set(false);
                    }
            );

            if (!illumination.get()) {
                wasManuallyIlluminated = true;
                illumination.set(true);
                xTimerStart(tmr, 0);
            }
        }

    private:

        static Property<bool> illumination;
        static bool wasManuallyIlluminated;

    };

    Property<bool> Matrix::illumination{};
    bool Matrix::wasManuallyIlluminated = false;

}

using matrix::Matrix;


#endif //ALARM_CLOCK_R2V6_MATRIX_H
