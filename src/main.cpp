//#define NDEBUG

#include <Arduino.h>
#include <AlarmClock.h>


void navigationCallback(const Navigation::PadType &pad);
Navigation navigation{navigationCallback};
MainLight mainLight{};
LightSensor lightSensor{};

void setup() {
    DEBUG_INIT(115200);
    DEBUG("setup begin");

    navigation.setup();
    mainLight.setup();
    lightSensor.setup();

    mainLight.setDuration(MainLight::Duration::MIN_5);
    lightSensor.getLightLevelProperty().addChangeListener([](const Property<float> &, float, float v) {
        DEBUG_F("lightLevel changed to %f", v);
    });

    DEBUG("setup end");
}

void loop() {
    navigation.loop();
    lightSensor.readLightLevel();
}

/**
 * Callback for navigation touchpad
 * @param pad the pad that was touched
 */
void navigationCallback(const Navigation::PadType &pad) {
    switch (pad) {
        case Navigation::PadType::MID:
            DEBUG("pad touched: MID");
            break;
        case Navigation::PadType::LEFT:
            DEBUG("pad touched: LEFT");
            break;
        case Navigation::PadType::RIGHT:
            DEBUG("pad touched: RIGHT");
            break;
        case Navigation::PadType::UP:
            DEBUG("pad touched: UP");
            mainLight.incrDuty();
            break;
        case Navigation::PadType::DOWN:
            DEBUG("pad touched: DOWN");
            mainLight.decrDuty();
            break;
    }
}
