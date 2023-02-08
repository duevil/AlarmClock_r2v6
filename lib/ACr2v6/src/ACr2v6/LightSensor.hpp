//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
#define ALARM_CLOCK_R2V6_LIGHT_SENSOR_H

#include <hp_BH1750.h>
#include <array>


struct LightSensor {

    static void init() {
        assert(!_init);
        _init = true;
        sensor.begin(BH1750_TO_GROUND);
        sensor.calibrateTiming();
        sensor.setQuality(BH1750_QUALITY_HIGH2);
        sensor.start();
        for (int i = 0; i < acc::LIGHT_SENSOR_READINGS; ++i) {
            while (!sensor.hasValue(true));
            readings[i] = sensor.getLux();
            sensor.start();
        }
    }

    static float read() {
        static uint8_t index;
        static float lastValue;

        assert(_init);

        if (sensor.hasValue()) {
            readings[index] = sensor.getLux();
            index = (index + 1) % acc::LIGHT_SENSOR_READINGS;
            lastValue = util::average<float, acc::LIGHT_SENSOR_READINGS>(readings);
            sensor.adjustSettings(acc::LIGHT_SENSOR_ADJUSTMENT, true);
            sensor.start();
        }

        return lastValue;
    }

private:

    static bool _init;
    static hp_BH1750 sensor;
    static std::array<float, acc::LIGHT_SENSOR_READINGS> readings;

};

bool LightSensor::_init = false;
hp_BH1750 LightSensor::sensor{};
std::array<float, acc::LIGHT_SENSOR_READINGS> LightSensor::readings{};

#endif //ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
