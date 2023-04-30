//
// Created by Malte on 29.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_DEBUG_H
#define ALARM_CLOCK_R2V6_DEBUG_H

#ifndef NDEBUG

/**
 * @brief Returns the current time in milliseconds minus the given offset.
 * @param offset the statically stored offset; defaults to 0
 * @return the current time in milliseconds minus the given offset
 */
int64_t ac_debug_millis(int64_t offset = 0) {
    static int64_t t_offset{offset};
    return esp_timer_get_time() / 1000LL - t_offset;
}

/**
 * @brief Initializes the serial connection and run a startup delay of 2.5 seconds.
 * The startup delay is used to prevent rapid rebooting of the ESP32 on system failure.
 *
 * After the startup delay, the elapsed milliseconds are stored as the offset for the millisecond to be printed in the debug messages.
 * @param ... the parameters passed to Serial.begin(...)
 */
#define DEBUG_INIT(...) do {                \
    Serial.begin(__VA_ARGS__);              \
    for (int i = 0; i < 10; ++i) {          \
        Serial.print('.');                  \
        delay(250);                         \
    }                                       \
    Serial.println();                       \
    ac_debug_millis(                        \
            esp_timer_get_time() / 1000LL); \
} while (0)

/**
 * @brief Prints a debug message to the serial connection.
 * The message is prefixed with the current time in milliseconds and the function name, using the __ASSERT_FUNC macro.
 * @param ... the parameters passed to Serial.print(...)
 */
#define DEBUG(...) do {                    \
    Serial.printf("[%lld] %s: ",           \
            ac_debug_millis(),             \
            __ASSERT_FUNC);                \
    Serial.print(__VA_ARGS__);             \
    Serial.println();                      \
} while (0)

/**
 * @brief Prints a formatted debug message to the serial connection.
 * The message is prefixed with the current time in milliseconds and the function name, using the __ASSERT_FUNC macro.
 * @param ... the parameters passed to Serial.printf(...)
 */
#define DEBUG_F(...) do {                  \
    Serial.printf("[%lld] %s: ",           \
            ac_debug_millis(),             \
            __ASSERT_FUNC);                \
    Serial.printf(__VA_ARGS__);            \
    Serial.println();                      \
} while (0)

#else
#define DEBUG_INIT(...) (void)0
#define DEBUG(...) (void)0
#define DEBUG_F(...) (void)0
#endif

#endif //ALARM_CLOCK_R2V6_DEBUG_H
