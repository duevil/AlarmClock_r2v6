//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_MAIN_LIGHT_DURATION_H
#define ALARM_CLOCK_MAIN_LIGHT_DURATION_H


namespace AlarmClock {

    enum class MainLightDurationEnum : uint8_t {
        MIN_5 = 5,
        MIN_10 = 10,
        MIN_15 = 15,
        MIN_20 = 20,
        MIN_30 = 30,
        MIN_45 = 45,
        MIN_60 = 60,
        MIN_90 = 90,
        MIN_120 = 120,
        MIN_180 = 180,
        MIN_240 = 240,
    };


    class MainLightDuration {

        static uint8_t mldToUint(MainLightDurationEnum mld) { return static_cast<uint8_t>(mld); }

        uint8_t duration{mldToUint(MainLightDurationEnum::MIN_30)};

    public:

        void operator++() {
            duration++;
        }

        void operator--() {
            duration--;
        }

        MainLightDuration &operator=(uint8_t d) {
            duration = d;
            return *this;
        }

        MainLightDuration &operator=(MainLightDurationEnum d) {
            this->duration = mldToUint(d);
            return *this;
        }

        uint8_t get() const { return static_cast<uint8_t>(duration); }

        bool operator==(MainLightDurationEnum d) const { return duration == mldToUint(d); }

        bool operator!=(MainLightDurationEnum d) const { return duration != mldToUint(d); }

        bool operator<(MainLightDurationEnum d) const { return duration < mldToUint(d); }

        bool operator>(MainLightDurationEnum d) const { return duration > mldToUint(d); }

        uint32_t toMS() const { return static_cast<uint32_t>(duration) * 60 * 1000; }
    };
}


#endif //ALARM_CLOCK_MAIN_LIGHT_DURATION_H
