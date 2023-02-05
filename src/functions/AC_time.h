//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_TIME_H
#define ALARM_CLOCK_R2V6_TIME_H

#include "RTClib.h"

#ifndef AC_TIME_NOW
#error "function for getting the current DateTime hast not been defined"
#endif

namespace ac_time {
    __attribute__((always_inline)) inline DateTime now() {
        return AC_TIME_NOW;
    }
}

#endif //ALARM_CLOCK_R2V6_TIME_H
