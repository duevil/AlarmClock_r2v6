//
// Created by Malte on 07.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_HPP
#define ALARM_CLOCK_R2V6_HPP

/* Often used dependencies */
#include <Arduino.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <debug.h>
#include <util.h>
#include <timeAC.h>
#include <vector>
#include <array>
#include <map>
#include <functional>
#include <memory>
/* Library classes and headers */
#include "ACr2v6/Constants.h"
#include "ACr2v6/Property.hpp"
#include "ACr2v6/Initialisable.hpp"
#include "ACr2v6/RTC.hpp"
#include "ACr2v6/Touchpad/touchpad.h"
#include "ACr2v6/LightSensor.hpp"
#include "ACr2v6/Matrix/Matrix.hpp"
#include "ACr2v6/LEDC/LEDC.hpp"
#include "ACr2v6/Storage.hpp"
#include "ACr2v6/Alarm/Alarm.hpp"
#include "ACr2v6/UserInterface/UserInterface.hpp"


namespace ac_r2v6 {

    /**
     * @brief The main Alarm Clock implementation
     */
    class AlarmClock final : private Initialisable {

    public:

        /**
         * @brief Singleton pattern
         * @return The AlarmClock instance
         */
        static AlarmClock &getInstance() {
            static AlarmClock instance{};
            return instance;
        };

        AlarmClock(AlarmClock &) = delete;
        AlarmClock &operator=(AlarmClock &) = delete;

        /**
         * @brief Initializes the Alarm Clock's elements
         */
        void init() override {
            setInit();
            touchpadHandler.init();
            lightSensor.init();
            matrix.init();
            ledc.init();
            // needs to be called after all other initializations were done to ensure that
            // all change listeners will be getting notified of possible changes during loading stored values
            storage.init();

#           ifndef NDEBUG
            for (const auto &item: properties) DEBUG(String{item.first} + ": ", item.second.get());
#           endif
        }

        /**
         * @brief Updates the elements readings and values
         */
        void run() {
            assert(isInit());
            touchpadHandler.run();
            lightValue.set(lightSensor.read());
            nowDT.set(now());
        }

    private:

        LEDC ledc{};
        Matrix matrix{lightValue, nowDT, ledc.valueProperty()};
        TouchpadHandler touchpadHandler{[this](pad_t pad) { onTouchpadPress(pad); }};
        LightSensor lightSensor{};
        RTC rtc{};
        Storage storage{properties, acc::PREFERENCES_NAME};

        Property<float> lightValue{};
        Property<DateTime> nowDT{};
        Property<uint8_t> a1_h{};
        Property<uint8_t> a1_m{};
        Property<uint8_t> a1_r{};
        Property<uint8_t> a1_s{};
        Property<uint8_t> a1_a{};
        Property<uint8_t> a2_h{};
        Property<uint8_t> a2_m{};
        Property<uint8_t> a2_r{};
        Property<uint8_t> a2_s{};
        Property<uint8_t> a2_a{};
        Property<uint8_t> pVol{};
        const Property<uint8_t>::Map properties = {
                {"a1_h", a1_h},
                {"a1_m", a1_m},
                {"a1_r", a1_r},
                {"a1_s", a1_s},
                {"a1_a", a1_a},
                {"a2_h", a2_h},
                {"a2_m", a2_m},
                {"a2_r", a2_r},
                {"a2_s", a2_s},
                {"a2_a", a2_a},
                {"vol",  pVol},
                {"onTm", ledc.durationProperty()},
        };

        /**
         * @brief Private constructor for singleton pattern
         */
        AlarmClock() = default;

        /**
         * @return The AlarmClock's current DateTime
         */
        DateTime now() { return rtc.now(); }

        /**
         * @brief Pressed listener method for the touchpad
         * @param pad The pressed pad enum value
         */
        void onTouchpadPress(pad_t pad) {
            DEBUG("Touched pad: ", pad_to_string(pad));
            if (matrix.isIlluminated()) {
                switch (pad) {
                    case pad_t::MID:
                    case pad_t::LEFT:
                    case pad_t::RIGHT:
                        break;
                    case pad_t::UP:
                        ++ledc.valueProperty();
                        break;
                    case pad_t::DOWN:
                        --ledc.valueProperty();
                        break;
                }
            } else matrix.illuminate();
        }

    };

}

/**
 * @brief Initializes the AlarmClock
 */
inline void alarm_clock_init() { ac_r2v6::AlarmClock::getInstance().init(); }

/**
 * @brief Runs the AlarmClock's updates
 */
inline void alarm_clock_run() { ac_r2v6::AlarmClock::getInstance().run(); }


#endif //ALARM_CLOCK_R2V6_HPP
