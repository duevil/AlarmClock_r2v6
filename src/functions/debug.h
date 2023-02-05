//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_DEBUG_H
#define ALARM_CLOCK_R2V6_DEBUG_H

#include <Arduino.h>

namespace debug {

    template<typename T>
    __attribute__((always_inline)) inline void print(T value) {
#if AC_R2V6_DEBUG
        Serial.printf("DEBUG[%lu] ", millis());
        Serial.println(value);
#endif
    }

    __attribute__((always_inline)) inline void init() {
#if AC_R2V6_DEBUG
        Serial.begin(115200);
#endif
    }

}

#endif //ALARM_CLOCK_R2V6_DEBUG_H
