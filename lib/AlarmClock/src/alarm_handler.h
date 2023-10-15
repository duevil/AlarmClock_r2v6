//
// Created by Malte on 09.10.2023.
//

#ifndef ALARM_CLOCK_ALARM_HANDLER_H
#define ALARM_CLOCK_ALARM_HANDLER_H


namespace AlarmClock {

    void stopAlarms();

    const ESP32_Timer alarmTurnOffTimer{
            "Turn Off Timer",
            30 * 60 * 1000 /* 30 min */,
            false,
            []() { stopAlarms(); }
    };

    /**
     * @brief Stops all alarms and resets the alarm if needed\n
     * Should be called when the alarm toggle is switched off. Stops all alarms, turns off the indicator light and
     * resets the alarm if needed. Also stops the alarm turn off timer.\n
     * If an alarm that is stopped as no repeat flags set, it is disabled, otherwise it is set again
     * at the next time it is supposed to go off.
     */
    void stopAlarms() {
        AC.player.stop();
        AC.indicatorLight.toggleOff();
        if (AC.alarm1.state == AlarmState::PLAYING || AC.alarm1.state == AlarmState::SNOOZED) {
            AC.alarm1.state = AlarmState::OFF;
            if ((uint8_t) AC.alarm1.repeat == 0) {
                AC.alarm1.toggle = false;
                disableAlarm(AC.alarm1, AC.rtc);
            } else {
                AC.alarm1.state = AlarmState::OFF;
                setAlarm(AC.alarm1, AC.rtc);
            }
        }
        if (AC.alarm2.state == AlarmState::PLAYING || AC.alarm2.state == AlarmState::SNOOZED) {
            AC.alarm2.state = AlarmState::OFF;
            if ((uint8_t) AC.alarm2.repeat == 0) {
                AC.alarm2.toggle = false;
                disableAlarm(AC.alarm2, AC.rtc);
            } else {
                AC.alarm2.state = AlarmState::OFF;
                setAlarm(AC.alarm2, AC.rtc);
            }
        }
        alarmTurnOffTimer.stop();
    }

    /**
     * @brief Snoozes both alarms if they are playing\n
     * Should be called when the snooze button is pressed. Stops the alarm sound, turns off the indicator light and
     * snoozes both alarms if they are playing. Sets the alarm state to SNOOZED and stops the alarm turn off timer,
     * as snooze itself is set as an alarm and would otherwise be stopped by the turn off timer.
     */
    void snoozeAlarms() {
        AC.player.stop();
        AC.indicatorLight.setDuty(1);
        alarmTurnOffTimer.stop();
        if (AC.alarm1.state == AlarmState::PLAYING &&
            snoozeAlarm(AC.alarm1, AC.rtc, (int8_t) AC.snoozeTime)) {
            AC.alarm1.state = AlarmState::SNOOZED;
        }
        if (AC.alarm2.state == AlarmState::PLAYING &&
            snoozeAlarm(AC.alarm2, AC.rtc, (int8_t) AC.snoozeTime)) {
            AC.alarm2.state = AlarmState::SNOOZED;
        }
    }

    /**
     * @brief Handles alarm triggers\n
     * Should be called when the RTC alarm interrupt is triggered. Reads both alarms and handles them accordingly,
     * i.e. plays the alarm sound, turns on the indicator light and the main light and switches to the alarm frame.
     * Also starts the alarm turn off timer to disable the alarm after 30 minutes and sets the defuse code.
     */
    void handleAlarms() {
        if (!AC.anyAlarmTriggered) return;
        AC.anyAlarmTriggered = false;
        if (readAlarm(AC.alarm1, AC.rtc)) {
            auto sound = (uint8_t) AC.alarm1.sound;
            if (sound == 0) AC.player.playLoop(Sound::findRandomSound(AC.sounds).getId());
            else AC.player.playLoop(sound);
        }
        if (readAlarm(AC.alarm2, AC.rtc)) {
            auto sound = (uint8_t) AC.alarm2.sound;
            if (sound == 0) AC.player.playLoop(Sound::findRandomSound(AC.sounds).getId());
            else AC.player.playLoop(sound);
        }
        alarmTurnOffTimer.start();
        AC.indicatorLight.toggleOn();
        AC.mainLight.toggleOn();
        AC.ui.transitionToFrame(1); // alarm frame
        for (auto &i: AC.defuseCode) i = (uint8_t) random(4);
        AC.uiTimer.stop();
    }


}


#endif //ALARM_CLOCK_ALARM_HANDLER_H
