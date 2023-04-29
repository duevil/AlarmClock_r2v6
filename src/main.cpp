//#define NDEBUG

#include <Arduino.h>
#include <AlarmClock.h>


void navigationCallback(const Navigation::pad_t &pad);
const Navigation navigation{navigationCallback};

void setup() {
    DEBUG_INIT(115200);
    DEBUG("setup begin");

    navigation.setup();

    DEBUG("setup end");
}

void loop() {
    navigation.loop();
}

/**
 * Callback for navigation touchpad
 * @param pad the pad that was touched
 */
void navigationCallback(const Navigation::pad_t &pad) {
    switch (pad) {
        case Navigation::pad_t::MID:
            DEBUG("MID");
            break;
        case Navigation::pad_t::LEFT:
            DEBUG("LEFT");
            break;
        case Navigation::pad_t::RIGHT:
            DEBUG("RIGHT");
            break;
        case Navigation::pad_t::UP:
            DEBUG("UP");
            break;
        case Navigation::pad_t::DOWN:
            DEBUG("DOWN");
            break;
    }
}
