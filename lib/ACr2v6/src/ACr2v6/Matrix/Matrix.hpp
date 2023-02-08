//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MATRIX_H
#define ALARM_CLOCK_R2V6_MATRIX_H

#include <MD_Parola.h>
#include <string>
#include "font.h"


namespace matrix {
    class Matrix {

    public:

        static void init(Property<float> &lightProperty,Property<DateTime> &timeProperty) {
            static constexpr uint8_t CS_PIN = 5;
            static constexpr uint8_t NUM_DEVICES = 4;
            static constexpr float FACTOR = 0.2f;
            static bool _init = false;
            static MD_Parola md{MD_MAX72XX::FC16_HW, CS_PIN, NUM_DEVICES};

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
                intensity = (uint8_t) (lum * FACTOR);
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
            wasManuallyIlluminated = true;
            manualIlluminationStart = millis();
            illumination.set(true);
        }

        static void updateIllumination() {
            if (wasManuallyIlluminated && millis() - manualIlluminationStart > 2000 && intensity == 0) {
                wasManuallyIlluminated = false;
                illumination.set(false);
            }
        }

    private:

        static Property<bool> illumination;
        static uint8_t intensity;
        static bool wasManuallyIlluminated;
        static uint64_t manualIlluminationStart;

    };
}

Property<bool> matrix::Matrix::illumination{};
uint8_t matrix::Matrix::intensity = 0;
bool matrix::Matrix::wasManuallyIlluminated = false;
uint64_t matrix::Matrix::manualIlluminationStart = millis();

using matrix::Matrix;

#endif //ALARM_CLOCK_R2V6_MATRIX_H
