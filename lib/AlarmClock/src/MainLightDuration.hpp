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

        MainLightDurationEnum duration{MainLightDurationEnum::MIN_30};

    public:

        void operator++() {
            switch (duration) {
                case MainLightDurationEnum::MIN_5:
                    duration = MainLightDurationEnum::MIN_10;
                    break;
                case MainLightDurationEnum::MIN_10:
                    duration = MainLightDurationEnum::MIN_15;
                    break;
                case MainLightDurationEnum::MIN_15:
                    duration = MainLightDurationEnum::MIN_20;
                    break;
                case MainLightDurationEnum::MIN_20:
                    duration = MainLightDurationEnum::MIN_30;
                    break;
                case MainLightDurationEnum::MIN_30:
                    duration = MainLightDurationEnum::MIN_45;
                    break;
                case MainLightDurationEnum::MIN_45:
                    duration = MainLightDurationEnum::MIN_60;
                    break;
                case MainLightDurationEnum::MIN_60:
                    duration = MainLightDurationEnum::MIN_90;
                    break;
                case MainLightDurationEnum::MIN_90:
                    duration = MainLightDurationEnum::MIN_120;
                    break;
                case MainLightDurationEnum::MIN_120:
                    duration = MainLightDurationEnum::MIN_180;
                    break;
                case MainLightDurationEnum::MIN_180:
                    duration = MainLightDurationEnum::MIN_240;
                    break;
                case MainLightDurationEnum::MIN_240:
                    duration = MainLightDurationEnum::MIN_5;
                    break;
            }
        }

        void operator--() {
            switch (duration) {
                case MainLightDurationEnum::MIN_5:
                    duration = MainLightDurationEnum::MIN_240;
                    break;
                case MainLightDurationEnum::MIN_10:
                    duration = MainLightDurationEnum::MIN_5;
                    break;
                case MainLightDurationEnum::MIN_15:
                    duration = MainLightDurationEnum::MIN_10;
                    break;
                case MainLightDurationEnum::MIN_20:
                    duration = MainLightDurationEnum::MIN_15;
                    break;
                case MainLightDurationEnum::MIN_30:
                    duration = MainLightDurationEnum::MIN_20;
                    break;
                case MainLightDurationEnum::MIN_45:
                    duration = MainLightDurationEnum::MIN_30;
                    break;
                case MainLightDurationEnum::MIN_60:
                    duration = MainLightDurationEnum::MIN_45;
                    break;
                case MainLightDurationEnum::MIN_90:
                    duration = MainLightDurationEnum::MIN_60;
                    break;
                case MainLightDurationEnum::MIN_120:
                    duration = MainLightDurationEnum::MIN_90;
                    break;
                case MainLightDurationEnum::MIN_180:
                    duration = MainLightDurationEnum::MIN_120;
                    break;
                case MainLightDurationEnum::MIN_240:
                    duration = MainLightDurationEnum::MIN_180;
                    break;
            }
        }

        MainLightDuration &operator=(uint8_t d) {
            if (d < 8) duration = MainLightDurationEnum::MIN_5;
            else if (d < 13) duration = MainLightDurationEnum::MIN_10;
            else if (d < 18) duration = MainLightDurationEnum::MIN_15;
            else if (d < 23) duration = MainLightDurationEnum::MIN_20;
            else if (d < 38) duration = MainLightDurationEnum::MIN_30;
            else if (d < 53) duration = MainLightDurationEnum::MIN_45;
            else if (d < 68) duration = MainLightDurationEnum::MIN_60;
            else if (d < 98) duration = MainLightDurationEnum::MIN_90;
            else if (d < 128) duration = MainLightDurationEnum::MIN_120;
            else if (d < 193) duration = MainLightDurationEnum::MIN_180;
            else duration = MainLightDurationEnum::MIN_240;
            return *this;
        }

        MainLightDuration &operator=(MainLightDurationEnum d) {
            this->duration = d;
            return *this;
        }

        uint8_t get() const { return static_cast<uint8_t>(duration); }

        bool operator==(MainLightDurationEnum d) const { return duration == d; }

        bool operator!=(MainLightDurationEnum d) const { return duration != d; }

        bool operator<(MainLightDurationEnum d) const { return duration < d; }

        bool operator>(MainLightDurationEnum d) const { return duration > d; }

        uint32_t toMS() const { return static_cast<uint32_t>(duration) * 60 * 1000; }
    };
}


#endif //ALARM_CLOCK_MAIN_LIGHT_DURATION_H
