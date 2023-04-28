//#define NDEBUG

#include <Arduino.h>
#include "ACr2v6/ACr2v6.h"

Property<uint8_t> a1_h;
Property<uint8_t> a1_m;
Property<uint8_t> a1_r;
Property<uint8_t> a1_s;
Property<uint8_t> a1_a;
Property<uint8_t> a2_h;
Property<uint8_t> a2_m;
Property<uint8_t> a2_r;
Property<uint8_t> a2_s;
Property<uint8_t> a2_a;
Property<uint8_t> pVol;
Property<float> lightValue;
Property<DateTime> now;
const std::map<const char *const, Property<uint8_t> &> properties = {
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
        {"onTm", LEDC::durationProperty},
};

void onTouchpadPress(Touchpad::pad_t);

void setup() {
    DEBUG_INIT();
    DEBUG_SIMPLE("Setup start");

    RTC::init();
    Touchpad::init(onTouchpadPress);
    LightSensor::init();
    Matrix::init(lightValue, now, LEDC::lightProperty);
    LEDC::init();
    // needs to be called after all other initializations were done to ensure that
    // all change listeners will be getting notified of possible changes during loading
    Storage::init(properties);

#   ifndef NDEBUG
    for (const auto &item: properties) {
        char msg[10];
        strcpy(msg, item.first);
        strcat(msg, ": ");
        DEBUG(msg, item.second.get());
    }
#   endif

    DEBUG_SIMPLE("Setup end");
}

void loop() {
    Touchpad::read();
    auto value{LightSensor::read()};
    auto nowDT{ac_time::now()};

    lightValue.set(value);
    now.set(nowDT);
}

void onTouchpadPress(Touchpad::pad_t pad) {
    DEBUG("Touched pad: ", Touchpad::toString(pad));
    if (Matrix::isIlluminated()) {
        switch (pad) {
            case Touchpad::pad_t::MID:
            case Touchpad::pad_t::LEFT:
            case Touchpad::pad_t::RIGHT:
                break;
            case Touchpad::pad_t::UP:
                ++LEDC::lightProperty;
                break;
            case Touchpad::pad_t::DOWN:
                --LEDC::lightProperty;
                break;
        }
    } else Matrix::illuminate();
}