//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_LIGHT_DURATION_H
#define ALARM_CLOCK_MAIN_LIGHT_DURATION_H


namespace AlarmClock {

    /**
     * @brief Class that handles the main light and its duration
     */
    class MainLight {

        Uint8Bean duration;
        LEDC mainLight{LIGHT_PIN, LEDC::Resolution::BITS_3};
        const ESP32_Timer timer{
                "Main Light Timer",
                1000,
                false,
                [this]() { mainLight.toggleOff(); }
        };

    public:

        explicit MainLight(Preferences &preferences) : duration("lightDuration", preferences) {}

        /**
         * @brief Sets up the main light and loads the duration from the preferences
         */
        void setup() {
            mainLight.setup();
            duration.load();
            if ((uint8_t) duration) timer.changePeriod((uint8_t) duration * 60 * 1000);
        }

        /**
         * @brief Toggles the main light on, i.e. setting the duty to ist max value,
         * and resets the timer if the duration is set
         */
        void toggleOn() {
            if ((uint8_t) duration) timer.reset();
            mainLight.toggleOn();
        }

        /**
         * @brief Toggles the main light off and stops the timer
         */
        void toggleOff() {
            timer.stop();
            mainLight.toggleOff();
        }

        /**
         * @brief Sets the duration to the given value and changes the timer period accordingly
         * @param min The duration in minutes
         */
        void setDuration(uint8_t min) {
            duration = min;
            if (min > 0) timer.changePeriod(min * 60 * 1000);
        }

        /**
         * @brief Increases the duration by 5 minutes if it is less than 30 minutes, by 10 minutes if it is less than
         * 90 minutes and by 30 minutes otherwise
         */
        void incrDuration() {
            auto durVal = (uint8_t) duration;
            if (durVal < 30) duration = durVal + 5;
            else if (durVal < 90) duration = durVal + 10;
            else duration = durVal + 30;
        }

        /**
         * @brief Decreases the duration by 5 minutes if it is greater than 30 minutes, by 10 minutes if it is greater
         * than 90 minutes and by 30 minutes otherwise
         */
        void decrDuration() {
            auto durVal = (uint8_t) duration;
            if (durVal >= 90) duration = durVal - 30;
            else if (durVal >= 30) duration = durVal - 10;
            else duration = durVal - 5;
        }

        /**
         * @brief Returns the duration in minutes
         * @return The duration in minutes
         */
        uint8_t getDuration() const { return (uint8_t) duration; }

        /**
         * @brief Sets the duty to the given value and resets the timer if the duty is greater than 0
         * @param duty The duty to set
         */
        void setDuty(uint8_t duty) {
            mainLight.setDuty(duty);
            if (duty && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        /**
         * @brief Increases the duty by 1 and resets the timer if the duty is greater than 0
         */
        void incrDuty() {
            ++mainLight;
            if (mainLight.getDuty() && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        /**
         * @brief Decreases the duty by 1 and resets the timer if the duty is greater than 0
         */
        void decrDuty() {
            --mainLight;
            if (mainLight.getDuty() && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        /**
         * @brief Returns the duty
         * @return The duty
         */
        uint32_t getDuty() const { return mainLight.getDuty(); }

        /**
         * @brief Toggles the main light on or off depending on the current duty
         */
        void toJson(JsonVariant &json) {
            json["duty"] = mainLight.getDuty();
            json["duration"] = getDuration();
        }

        /**
         * @brief Sets the duty to the given value and resets the timer if the duty is greater than 0
         * @param duty The duty to set
         */
        void fromJson(JsonVariant &json) {
            setDuty(json["duty"].as<uint8_t>());
            setDuration(json["duration"].as<uint8_t>());
        }

        // delete copy constructor and assignment operator

        MainLight(const MainLight &) = delete;

        MainLight &operator=(const MainLight &) = delete;

    };
}


#endif //ALARM_CLOCK_MAIN_LIGHT_DURATION_H
