//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_LIGHT_DURATION_H
#define ALARM_CLOCK_MAIN_LIGHT_DURATION_H


namespace AlarmClock {

    class MainLight {

        Uint8Bean duration;
        LEDC mainLight{LIGHT_PIN, LEDC::Resolution::BITS_3};
        ESP32_Timer timer{
                "Main Light Timer",
                1000,
                false,
                [this]() { mainLight.toggleOff(); }
        };

    public:

        explicit MainLight(Preferences &preferences) : duration("lightDuration", preferences) {}

        void setup() {
            mainLight.setup();
            duration.load();
        }

        void toggleOn() {
            if ((uint8_t) duration) timer.reset();
            mainLight.toggleOn();
        }

        void toggleOff() {
            timer.stop();
            mainLight.toggleOff();
        }

        void setDuration(uint8_t min) {
            duration = min;
            if (min > 0) timer.changePeriod(min * 60 * 1000);
        }

        uint8_t getDuration() const { return (uint8_t) duration; }

        void setDuty(uint8_t duty) {
            mainLight.setDuty(duty);
            if (duty && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        void incrDuty() {
            ++mainLight;
            if (mainLight.getDuty() && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        void decrDuty() {
            --mainLight;
            if (mainLight.getDuty() && (uint8_t) duration) timer.reset();
            else timer.stop();
        }

        uint32_t getDuty() const { return mainLight.getDuty(); }

        void toJson(JsonVariant &json) {
            json["duty"] = mainLight.getDuty();
            json["duration"] = getDuration();
        }

        void fromJson(JsonVariant &json) {
            setDuty(json["duty"].as<uint8_t>());
            setDuration(json["duration"].as<uint8_t>());
        }
    };
}


#endif //ALARM_CLOCK_MAIN_LIGHT_DURATION_H
