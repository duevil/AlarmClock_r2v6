//
// Created by Malte on 29.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_MAIN_LIGHT_HPP
#define ALARM_CLOCK_R2V6_MAIN_LIGHT_HPP

#include "base/Property.hpp"
#include "esp32/LEDC.hpp"
#include "esp32/Timer.hpp"


/**
 * @brief Class to control the main light of the alarm clock.
 *
 * It implements a 2 bit linear brightness ledc and a timer to turn off the light after a certain time.
 */
class MainLight {

public:
    enum class Duty {
        OFF = 0,
        MIN = 1,
        MID = 2,
        MAX = 3,
    };

    enum class Duration {
        MIN_5 = 0,
        MIN_10 = 1,
        MIN_15 = 2,
        MIN_30 = 3,
        MIN_45 = 4,
        MIN_60 = 5,
        MIN_90 = 6,
        MIN_120 = 7,
        MIN_180 = 8,
        MIN_240 = 9,
    };

    MainLight() = default;

    /**
     * @brief Sets up the main light. Sets up the ledc
     * and attaches change listeners to the duty and duration properties for controlling the timer and ledc.
     */
    void setup() {
        ledc.setup();
        duty.addChangeListener([this](const Property<uint8_t> &, uint8_t, uint8_t v) {
            DEBUG_F("MainLight::dutyChangeListener() duty changed to %d", v);
            ledc.setDuty(LINEAR_DUTY_LUT[v]);
            if (v > 0) timer.reset();
            else timer.stop();
        });
        duration.addChangeListener([this](const Property<uint8_t> &, uint8_t, uint8_t v) {
            DEBUG_F("MainLight::durationChangeListener() duration changed to %d", v);
            timer.changePeriod(1000 * 60 * DURATION_LUT[v]);
            if (duty.get() > 0) timer.reset();
            else timer.stop();
        });
    }

    uint8_t getDuty() const { return duty.get(); }

    uint8_t getDuration() const { return duration.get(); }

    void setDuty(Duty val) { duty = static_cast<uint8_t>(val); }

    void setDuration(Duration val) { duration = static_cast<uint8_t>(val); }

    /**
     * @brief Increases the duty by one, wrapping around at the maximum value.
     */
    void incrDuty() { duty = (duty + 1) % LINEAR_DUTY_LUT.size(); }

    /**
     * @brief Decreases the duty by one, wrapping around at the minimum value.
     */
    void decrDuty() { duty = (duty - 1 + LINEAR_DUTY_LUT.size()) % LINEAR_DUTY_LUT.size(); }

    /**
     * @brief Increases the duration by one, wrapping around at the maximum value.
     */
    void incrDuration() { duration = (duration + 1) % DURATION_LUT.size(); }

    /**
     * @brief Decreases the duration by one, wrapping around at the minimum value.
     */
    void decrDuration() { duration = (duration - 1 + DURATION_LUT.size()) % DURATION_LUT.size(); }

    // disable copy constructor and assignment operator

    MainLight(const MainLight &) = delete;
    MainLight &operator=(const MainLight &) = delete;

private:
    static constexpr int LEDC_PIN = 25;
    static const std::array<uint8_t, 4> LINEAR_DUTY_LUT; // lookup table for 2 bit linear brightness
    static const std::array<uint8_t, 10> DURATION_LUT; // lookup table for duration in minutes

    Property<uint8_t> duty{"lightDuty"};
    Property<uint8_t> duration{"lightDur", static_cast<uint8_t>(Duration::MIN_30)};

    const LEDC ledc{LEDC_PIN, LEDC::Resolution::BITS_8};
    const Timer timer{
            "lightTimer",
            static_cast<uint32_t>(1000 * 60 * DURATION_LUT[duration.get()]),
            false,
            [this]() { duty = 0; }
    };

};

std::array<uint8_t, 4> constexpr const MainLight::LINEAR_DUTY_LUT = {0, 16, 64, 255};
std::array<uint8_t, 10> constexpr const MainLight::DURATION_LUT = {5, 10, 15, 30, 45, 60, 90, 120, 180, 240};


#endif //ALARM_CLOCK_R2V6_MAIN_LIGHT_HPP
