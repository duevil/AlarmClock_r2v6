//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
#define ALARM_CLOCK_R2V6_LIGHT_SENSOR_H

#include <Arduino.h>
#include "hp_BH1750.h"
#include <array>
#include "functions/util.h"

namespace light {

    float read() {
        static constexpr uint8_t NUM_READINGS = 5;
        static bool init = false;
        static hp_BH1750 sensor;
        static std::array<float, NUM_READINGS> readings;
        static uint8_t index;
        static float lastValue;

        if (!init) {
            init = true;
            sensor.begin(BH1750_TO_GROUND);
            sensor.calibrateTiming();
            sensor.setQuality(BH1750_QUALITY_HIGH2);
            sensor.start();
            for (int i = 0; i < NUM_READINGS; ++i) {
                while (!sensor.hasValue(true));
                readings[i] = sensor.getLux();
                sensor.start();
            }
        }

        if (sensor.hasValue()) {
            readings[index] = sensor.getLux();
            index = (index + 1) % NUM_READINGS;
            lastValue = util::average<float, NUM_READINGS>(readings);
            sensor.adjustSettings(90, true);
            sensor.start();
        }

        return lastValue;
    }

}

#endif //ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
