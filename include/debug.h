//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_DEBUG_H
#define ALARM_CLOCK_R2V6_DEBUG_H

#ifndef NDEBUG

namespace ac_debug_t {
    int64_t millis(int64_t offset = 0) {
        static int64_t t_offset{offset};
        return esp_timer_get_time() / 1000LL - t_offset;
    }
}

// init contains startup delay to avoid rapid reboots on system failures
#define DEBUG_INIT() do { \
    Serial.begin(SERIAL_BAUD); \
    for (uint8_t i{0}; i < 5 && Serial.print('.'); ++i) delay(500); \
    Serial.println(); \
    ac_debug_t::millis(esp_timer_get_time() / 1000LL); \
} while (0)
#define DEBUG_SIMPLE(value) do { \
    Serial.printf("DEBUG[%lld] ", ac_debug_t::millis()); \
    Serial.println(value); \
} while (0)
#define DEBUG(identifier, value) do { \
    Serial.printf("DEBUG[%lld] ", ac_debug_t::millis()); \
    Serial.print(identifier);\
    Serial.println(value); \
} while (0)
#define ERROR(message) do { \
    Serial.printf("ERROR[%lld] ", ac_debug_t::millis()); \
    Serial.print(message); \
} while (0)
#else
#define DEBUG_INIT() ((void)0)
#define DEBUG_SIMPLE(ignored) ((void)0)
#define DEBUG(ignored0, ignored1) ((void)0)
#define ERROR(ignored) ((void)0)
#endif

#endif //ALARM_CLOCK_R2V6_DEBUG_H
