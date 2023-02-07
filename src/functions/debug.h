//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_DEBUG_H
#define ALARM_CLOCK_R2V6_DEBUG_H

#if AC_R2V6_DEBUG
#define DEBUG_INIT() Serial.begin(115200)
#define DEBUG_SIMPLE(value) do { \
    Serial.printf("DEBUG[%lu] ", millis()); \
    Serial.println(value); \
} while (0)
#define DEBUG(identifier, value) do { \
    Serial.printf("DEBUG[%lu] ", millis()); \
    Serial.print(identifier);\
    Serial.println(value); \
} while (0)
#else
#define DEBUG_INIT() do {} while(0)
#define DEBUG_SIMPLE(value) do {} while(0)
#define DEBUG(identifier, value) do {} while(0)
#endif

#endif //ALARM_CLOCK_R2V6_DEBUG_H
