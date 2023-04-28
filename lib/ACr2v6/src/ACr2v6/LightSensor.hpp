//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
#define ALARM_CLOCK_R2V6_LIGHT_SENSOR_H

#include <hp_BH1750.h>


namespace ac_r2v6 {

    /**
     * Implements the reading of the Alarm Clock's light sensor
     */
    class LightSensor final : private Initialisable {

    public:

        /**
         * Initializes the physical sensor and calculates an average value;
         * depending on the amount of readings for averaging, this might take a while
         */
        void init() override {
            DEBUG_SIMPLE("BH1750 initialization start");
            setInit();

            sensor.begin(BH1750_TO_GROUND);
            sensor.calibrateTiming();
            sensor.setQuality(BH1750_QUALITY_HIGH2);
            sensor.start();
            for (int i{0}; i < readingsAmount; ++i) {
                while (!sensor.hasValue(true));
                readings[i] = sensor.getLux();
                sensor.start();
            }

            DEBUG_SIMPLE("BH1750 initialization end");
        }

        /**
         * Updates this sensors average value if a reading of the physical sensor is available
         * @return The average value
         */
        float read() {
            static uint8_t index;
            static float lastValue{-1.0F};

            if (isInit() && sensor.hasValue()) {
                readings[index] = sensor.getLux();
                index = (index + 1) % readingsAmount;
                lastValue = util::average<float, readingsAmount>(readings);
                sensor.adjustSettings(sensorAdjustment, true);
                sensor.start();
            }

            return lastValue;
        }

    private:

        constexpr static uint8_t readingsAmount = acc::LIGHT_SENSOR_READINGS;
        constexpr static uint8_t sensorAdjustment = acc::LIGHT_SENSOR_ADJUSTMENT;
        hp_BH1750 sensor{};
        std::array<float, readingsAmount> readings{};

    };

}

#endif //ALARM_CLOCK_R2V6_LIGHT_SENSOR_H
