# Notes regarding the different datetime objects

The alarm clock uses two different datetime objects:

- The datetime stored on the DS3231 RTC, represented by the RTCLib DateTime class
- The DateTime stored which is retrieved from the NTP server and stored in the ESP32 internal RTC, represented by the c-library time_t type 

These two datetime objects are using different time notations:

- The DS3231 or objects of the DateTime class ar noted in seconds since 2000-01-01 00:00:00 without any time zone information, thus being considered to be in the local time zone.
- The NTP datetime objects are noted in seconds since 1970-01-01 00:00:00 UTC.

Therefore, when comparing the two datetime objects, the time zone information has to be taken into account, i.e. the time zone of the ESP32 RTC has to be added to the DS3231 datetime object.

In principle, only the time of the DS3231 rtc should be used and the NTP time should only be used to adjust the DS3231 time. This can be done in the following way:

```c++
// configure the internal rtc over NTP
// this has to be done only once
const char* tz = "...";
const char *ntpServer = "...";
configTzTime(tz, ntpServer);

// get the current time from the internal rtc
struct tm timeinfo{};
if (!getLocalTime(&timeinfo, 100)) {
    // handle error
}

// set the DS3231 time
DateTime dt{
    timeinfo.tm_year + 1900,
    timeinfo.tm_mon + 1,
    timeinfo.tm_mday,
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
};
ds3231.adjust(dt);
```

