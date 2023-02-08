//
// Created by Malte on 05.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_DEBUG_H
#define ALARM_CLOCK_R2V6_DEBUG_H

#ifndef NDEBUG
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
#define ERROR(message) do { \
    Serial.printf("ERROR[%lu] ", millis()); \
    Serial.print(message); \
} while (0)
#else
#define DEBUG_INIT() ((void)0)
#define DEBUG_SIMPLE(ignored) ((void)0)
#define DEBUG(ignored0, ignored1) ((void)0)
#define ERROR(ignored) ((void)0)
#endif

#endif //ALARM_CLOCK_R2V6_DEBUG_H
