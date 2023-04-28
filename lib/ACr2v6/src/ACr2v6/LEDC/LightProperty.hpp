//
// Created by Malte on 08.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_LIGHT_PROPERTY_HPP
#define ALARM_CLOCK_R2V6_LIGHT_PROPERTY_HPP


namespace ac_r2v6 {

    struct LightProperty : Property<__uint8_t> {

        void set(__uint8_t value) override { if (value < acc::LIGHT_DUTIES_SIZE) Property::set(value); }

        void turnMax() { set(acc::LIGHT_DUTIES[acc::LIGHT_DUTIES_SIZE - 1]); }

        void turnOff() { set(acc::LIGHT_DUTIES[0]); }

        void operator++() { set((get() + 1) % acc::LIGHT_DUTIES_SIZE); }

        void operator--() { set((acc::LIGHT_DUTIES_SIZE + get() - 1) % acc::LIGHT_DUTIES_SIZE); }
    };

}

#endif //ALARM_CLOCK_R2V6_LIGHT_PROPERTY_HPP
