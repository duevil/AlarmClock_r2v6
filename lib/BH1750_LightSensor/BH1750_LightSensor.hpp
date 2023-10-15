//
// Created by Malte on 30.04.2023.
//

#ifndef BH1750_LIGHT_SENSOR_HPP
#define BH1750_LIGHT_SENSOR_HPP


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
    bool setup() {
        assert(!setupDone);
        if (!sensor.begin(BH1750_TO_GROUND)) return false;
        sensor.calibrateTiming();
        sensor.setQuality(BH1750_QUALITY_HIGH2);
        sensor.adjustSettings(ADJUSTMENT);
        sensor.start();
        for (auto i = 0; i < READINGS; ++i) {
            if (sensor.hasValue(true)) value = sensor.getLux();
            sensor.start();
        }
        return setupDone = true;
    }

    /**
     * @brief Reads the light level from the BH1750 sensor. \n
     * This function is non-blocking and therefore should be called regularly.
     * @return True if a value was read and a new reading was started, false otherwise.
     */
    bool tryReading() {
        assert(setupDone);
        if (sensor.hasValue()) {
            value = sensor.getLux();
            return sensor.adjustSettings(ADJUSTMENT) && sensor.start();
        }
        return false;
    }

    float getValue() { return value; }

    // delete copy constructor and assignment operator

    LightSensor(const LightSensor &o) = delete;

    LightSensor &operator=(const LightSensor &o) = delete;

private:
    static constexpr auto READINGS = 15;
    static constexpr auto ADJUSTMENT = 75;

    bool setupDone{false};

    hp_BH1750 sensor{};
    AveragedValue<float, READINGS> value{};

};


#endif //BH1750_LIGHT_SENSOR_HPP
