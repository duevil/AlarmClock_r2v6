//
// Created by Malte on 04.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_MAIN_LIGHT_H
#define ALARM_CLOCK_R2V6_MAIN_LIGHT_H


class LEDC {

public:

    struct LightProperty : Property<uint8_t> {

        void set(uint8_t value) override { if (value < acc::LIGHT_DUTIES_SIZE) Property::set(value); }

        void turnMax() { set(acc::LIGHT_DUTIES[acc::LIGHT_DUTIES_SIZE - 1]); }

        void turnOff() { set(acc::LIGHT_DUTIES[0]); }

        void operator++() { set((get() + 1) % acc::LIGHT_DUTIES_SIZE); }

        void operator--() { set((acc::LIGHT_DUTIES_SIZE + get() - 1) % acc::LIGHT_DUTIES_SIZE); }
    };

    struct DurationProperty : Property<uint8_t> {

        void set(uint8_t value) override { if (value < acc::LIGHT_DURATIONS_SIZE) Property::set(value); }

        void operator++() { set((get() + 1) % acc::LIGHT_DURATIONS_SIZE); }

        void operator--() { set((acc::LIGHT_DURATIONS_SIZE + get() - 1) % acc::LIGHT_DURATIONS_SIZE); }

    };

    static LightProperty lightProperty;
    static DurationProperty durationProperty;

    static void init() {
        static bool _init{false};
        assert(!_init);

        static auto timer = xTimerCreate(
                "light timer",
                calcLightTicks(0),
                pdFALSE,
                nullptr,
                [](TimerHandle_t) { lightProperty.set(0); }
        );

        ledcSetup(acc::LIGHT_CHANNEL, acc::LIGHT_FREQUENCY, acc::LIGHT_RESOLUTION);
        ledcAttachPin(acc::LIGHT_PIN, acc::LIGHT_CHANNEL);

        lightProperty.addListener([](uint8_t, uint8_t d) {
            ledcWrite(acc::LIGHT_CHANNEL, acc::LIGHT_DUTIES[d]);
            if (d) {
                DEBUG("xTimer started: ", pcTimerGetTimerName(timer));
                DEBUG("Light will be turned off after [minutes]: ", acc::LIGHT_DURATIONS[durationProperty.get()]);
                xTimerReset(timer, 0);
            }
        });
        durationProperty.addListener([](uint8_t, uint8_t d) {
            auto state{xTimerIsTimerActive(timer)};
            xTimerChangePeriod(timer, calcLightTicks(d), 0);
            if (!state) xTimerStop(timer, 0);
        });

        _init = true;
    }

private:

    static constexpr uint32_t calcLightTicks(const uint8_t d) {
        return (acc::LIGHT_DURATIONS[d] * 60 * 1000) / portTICK_PERIOD_MS;
    }

};

LEDC::LightProperty LEDC::lightProperty{};
LEDC::DurationProperty LEDC::durationProperty{};


#endif //ALARM_CLOCK_R2V6_MAIN_LIGHT_H
