//#define NDEBUG

#include <Arduino.h>
#include <ACr2v6.h>


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
Property<uint8_t> lDur;
Property<float> lightValue;
Property<DateTime> now;
LightS<15> lightS;
const std::map<const char *, Property<uint8_t> const &> properties = {
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
        {"onTm", lDur},
};

__attribute__((used)) void setup() {
    DEBUG_INIT();
    for (uint8_t i = 0; i < 5 && Serial.print('.'); ++i) delay(500);
    DEBUG_SIMPLE("Setup start");

    Storage::init("ACr2v2", properties);
    AC_RTC::init();
    Touchpad::init();
    LightSensor::init();
    Matrix::init(lightValue, now);

    DEBUG_SIMPLE("Setup end");
}

__attribute__((used)) void loop() {
    // loop
    auto value = LightSensor::read();
    auto nowDT = ac_time::now();
    auto touched = Touchpad::read();

    lightValue.set(value);
    now.set(nowDT);
    if (touched) {
        Matrix::illuminate();
        DEBUG("Touched pad: ", touched->toString());
    }

    Matrix::updateIllumination();
}