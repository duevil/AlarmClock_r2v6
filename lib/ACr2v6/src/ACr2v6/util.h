//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_UTIL_H
#define ALARM_CLOCK_R2V6_UTIL_H

#include <array>

namespace util {

    template<typename T, uint8_t size>
    inline T average(const std::array<T, size> &readings) {
        return (T) (std::accumulate(readings.begin(), readings.end(), 0.0) / (double) size);
    }

}

#endif //ALARM_CLOCK_R2V6_UTIL_H
