//
// Created by Malte on 08.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_DURATION_PROPERTY_HPP
#define ALARM_CLOCK_R2V6_DURATION_PROPERTY_HPP


namespace ac_r2v6 {

    struct DurationProperty : Property<uint8_t> {

        void set(uint8_t value) override { if (value < acc::LIGHT_DURATIONS_SIZE) Property::set(value); }

        void operator++() { set((get() + 1) % acc::LIGHT_DURATIONS_SIZE); }

        void operator--() { set((acc::LIGHT_DURATIONS_SIZE + get() - 1) % acc::LIGHT_DURATIONS_SIZE); }

    };

}

#endif //ALARM_CLOCK_R2V6_DURATION_PROPERTY_HPP
