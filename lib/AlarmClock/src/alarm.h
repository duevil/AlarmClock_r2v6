//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_ALARM_H
#define ALARM_CLOCK_ALARM_H

// TODO: change functions to be member functions of Alarm


namespace AlarmClock {

    /**
     * Enum class for the alarm ordinal number\n
     * There are two alarms, ONE and TWO
     */
    enum class N : uint8_t {
        ONE = 1, TWO = 2
    };

    /**
     * Converts an alarm ordinal number to an integer
     * @param n The alarm ordinal number
     * @return The integer representation of the alarm ordinal number (1 or 2)
     */
    uint8_t nToInt(N n) { return static_cast<uint8_t>(n); }


    /**
     * Enum class for the alarm state\n
     * There are three states:\n
     * OFF - The alarm is not playing\n
     * SNOOZE - The alarm is snoozing\n
     * PLAYING - The alarm is playing
     */
    enum class AlarmState {
        OFF = 0, SNOOZE = 1, PLAYING = 2
    };


    /**
     * The alarm data struct\n
     * Contains the alarm time, the repeat days, the sound and the active state
     */
    struct Alarm {
        const N n;
        Uint8Bean hour; // 0-23
        Uint8Bean minute; // 0-59
        Uint8Bean repeat; // 0-127 - a bit for each day of the week, starting with Sunday
        BoolBean toggle; // true or false
        Uint8Bean sound; // 0-255
        AlarmState state{AlarmState::OFF};

        Alarm(N n, Preferences &preferences) :
                n{n},
                hour{n == N::ONE ? "A1H" : "A2H", preferences},
                minute{n == N::ONE ? "A1M" : "A2M", preferences},
                repeat{n == N::ONE ? "A1R" : "A2R", preferences},
                toggle{n == N::ONE ? "A1T" : "A2T", preferences},
                sound{n == N::ONE ? "A1S" : "A2S", preferences} {}
    };


    //#region Alarm specific functions

    /**
     * Sets up the given alarm
     * @param alarm The alarm to set up
     */
    void setupAlarm(Alarm &alarm) {
        alarm.hour.load();
        alarm.minute.load();
        alarm.repeat.load();
        alarm.toggle.load();
        alarm.sound.load();
    }

    /**
     * Returns the alarm time for the given alarm relative to the given time
     * @param alarm The alarm
     * @param now The time to calculate the alarm time relative to
     * @return The next DateTime when the alarm will go off
     */
    DateTime getAlarmTime(const Alarm &alarm, const DateTime &now) {
        if (!alarm.toggle) return {};

        DateTime alarmTime{
                now.year(),
                now.month(),
                now.day(),
                (uint8_t) alarm.hour,
                (uint8_t) alarm.minute,
                0
        };
        auto dayOfWeek = alarmTime.dayOfTheWeek();

        // If the alarm is not set to repeat, and the alarm time is in the past, return the next day
        if ((uint8_t) alarm.repeat == 0) {
            if (alarmTime < now) {
                alarmTime = alarmTime + TimeSpan(1, 0, 0, 0);
            }
            return alarmTime;
        }

        // If the alarm is set to repeat, and the alarm time is in the past, return the next weekday
        for (short i = 0; i < 7; ++i) {
            auto nextWeekday = (dayOfWeek + i) % 7;
            if ((uint8_t) alarm.repeat & 1 << nextWeekday) {
                alarmTime = alarmTime + TimeSpan(i, 0, 0, 0);
                if (alarmTime > now) return alarmTime;
            }
        }

        return {};
    }

    /**
     * Sets the alarm to go off in 8 hours from now
     * @param alarm The alarm to set
     * @param now The current time
     * @return A reference to the alarm
     */
    Alarm &setIn8hFromNow(Alarm &alarm, const DateTime &now) {
        auto alarmTime = now + TimeSpan(0, 8, 0, 0);
        alarm.hour = alarmTime.hour();
        alarm.minute = alarmTime.minute();
        alarm.repeat =(uint8_t) alarm.repeat == 0 ? 0
                : (uint8_t) ((uint8_t) alarm.repeat | 1 << alarmTime.dayOfTheWeek());
        alarm.toggle = true;
        return alarm;
    }

    //#endregion
    //#region Json functions

    /**
     * Adds the given alarm to a JsonVariant
     * @param alarm The alarm to parse to Json
     * @param json The JsonVariant to add the alarm data to
     * @param now The current time
     */
    void alarmToJson(const Alarm &alarm, const JsonVariant &json, const DateTime &now) {
        json["id"] = nToInt(alarm.n);
        json["hour"] = (uint8_t) alarm.hour;
        json["minute"] = (uint8_t) alarm.minute;
        json["repeat"] = (uint8_t) alarm.repeat;
        json["toggle"] = (bool) alarm.toggle;
        json["sound"] = (uint8_t) alarm.sound;
        auto alarmTime = getAlarmTime(alarm, now);
        json["nextDateTime"] = alarmTime != DateTime() ? alarmTime.unixtime() : 0;
    }

    /**
     * Parses the given JsonVariant to an alarm
     * @param alarm The alarm to parse the Json to
     * @param json The JsonVariant to parse
     */
    void alarmFromJson(Alarm &alarm, const JsonVariant &json) {
        alarm.hour = (uint8_t) json["hour"];
        alarm.minute = (uint8_t) json["minute"];
        alarm.repeat = (uint8_t) json["repeat"];
        alarm.toggle = (bool) json["toggle"];
        alarm.sound = (uint8_t) json["sound"];
    }

    //#endregion
    //#region DS3231 functions

    /**
     * Disables the given alarm on the DS3231 RTC
     * @param alarm The alarm to disable
     * @param rtc The DS3231 RTC
     */
    void disableAlarm(const Alarm &alarm, RTC_DS3231 &rtc) {
        rtc.disableAlarm(nToInt(alarm.n));
    }

    /**
     * Sets the alarm on the DS3231 RTC to the given alarm time
     * @param alarm The alarm to set
     * @param rtc The DS3231 RTC
     * @return True if the alarm was set successfully, false otherwise
     */
    bool setAlarm(const Alarm &alarm, RTC_DS3231 &rtc) {
        rtc.clearAlarm(nToInt(alarm.n));

        auto alarmTime = getAlarmTime(alarm, rtc.now());
        if (alarmTime == DateTime()) {
            disableAlarm(alarm, rtc);
            return true;
        }

        switch (alarm.n) {
            case N::ONE:
                return rtc.setAlarm1(alarmTime, DS3231_A1_Date);
            case N::TWO:
                return rtc.setAlarm2(alarmTime, DS3231_A2_Date);
        }

        return false;
    }

    /**
     * Reads the given alarm status on the DS3231 RTC
     * @param alarm The alarm to read
     * @param rtc The DS3231 RTC
     * @return True if the alarm went off, false otherwise
     */
    bool readAlarm(Alarm &alarm, RTC_DS3231 &rtc) {
        auto n = nToInt(alarm.n);
        if (rtc.alarmFired(n)) {
            alarm.state = AlarmState::PLAYING;
            rtc.clearAlarm(n);
            return true;
        }
        return false;
    }

    /**
     * Snoozes the given alarm on the DS3231 RTC for the given amount of minutes
     * @param alarm The alarm to snooze
     * @param rtc The DS3231 RTC
     * @param minutes The amount of minutes to snooze the alarm for
     * @return True if the alarm was snoozed successfully, false otherwise
     */
    bool snoozeAlarm(Alarm &alarm, RTC_DS3231 &rtc, int8_t minutes) {
        auto alarmTime = rtc.now() + TimeSpan(0, 0, minutes, 0);
        rtc.clearAlarm(nToInt(alarm.n));
        bool success;
        switch (alarm.n) {
            case N::ONE:
                success = rtc.setAlarm1(alarmTime, DS3231_A1_Date);
                break;
            case N::TWO:
                success = rtc.setAlarm2(alarmTime, DS3231_A2_Date);
                break;
        }
        if (success) {
            alarm.state = AlarmState::SNOOZE;
            return true;
        }
        return false;
    }
}


#endif //ALARM_CLOCK_ALARM_H
