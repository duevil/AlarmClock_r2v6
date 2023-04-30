//
// Created by Malte on 30.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_LIGHT_SENSOR_HPP
#define ALARM_CLOCK_R2V6_LIGHT_SENSOR_HPP

#include "base/AveragedValue.hpp"
#include "base/Property.hpp"


/**
 * @brief Class for reading the light level with the BH1750 sensor.
 */
class LightSensor {

public:
    LightSensor() = default;

    /**
     * @brief Sets up the BH1750 sensor.
     * Reads the sensor multiple times to get a good average value.
     */
    void setup() {
        DEBUG_F("LightSensor::setup() begin");
        bool success = sensor.begin(BH1750_TO_GROUND);
        assert(success && "LightSensor::setup() sensor.begin() failed");
        sensor.calibrateTiming();
        sensor.setQuality(BH1750_QUALITY_HIGH2);
        sensor.start();
        for (int i = 0; i < READINGS; ++i) {
            if (sensor.hasValue(true)) value = sensor.getLux();
            sensor.start();
        }
        lightLevel = value;
        DEBUG_F("LightSensor::setup() initial value=%f", static_cast<float>(value));
        DEBUG_F("LightSensor::setup() end");
    }

    /**
     * @brief Reads the light level from the BH1750 sensor.
     */
    void readLightLevel() {
        if (sensor.hasValue()) {
            auto lux = sensor.getLux();
            value = lux;
            lightLevel = value;
            sensor.adjustSettings(ADJUSTMENT);
            sensor.start();
        }
    }

    float getLightLevel() { return value; }

    Property<float> &getLightLevelProperty() { return lightLevel; }

    // delete copy constructor and assignment operator

    LightSensor(const LightSensor &other) = delete;
    LightSensor &operator=(const LightSensor &other) = delete;

private:
    static constexpr uint8_t READINGS = 15;
    static constexpr uint8_t ADJUSTMENT = 75;

    hp_BH1750 sensor{};
    AveragedValue<float, READINGS> value{};
    Property<float> lightLevel{"lightLevel"};

};


#endif //ALARM_CLOCK_R2V6_LIGHT_SENSOR_HPP
