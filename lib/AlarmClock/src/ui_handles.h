//
// Created by Malte on 09.10.2023.
//

#ifndef ALARM_CLOCK_UI_HANDLES_H
#define ALARM_CLOCK_UI_HANDLES_H


namespace AlarmClock {

    navigation::Direction getInput() {
        auto direction = navigation::read();
        if (direction != navigation::Direction::None
            && AC.alarm1.state != AlarmState::PLAYING
            && AC.alarm2.state != AlarmState::PLAYING)
            AC.uiTimer.reset();
        return direction;
    }

    // ui handle for the home frame
    void uiHome(UIDisplay &ui) {
        static const ESP32_Timer matrixScrollTimer{
                "Matrix Scroll Timer",
                10000,
                false,
                []() { AC.matrix.scrollToStart(); }
        };

        AC.uiActive = false;
        switch (getInput()) {
            case navigation::Direction::Center:
                if (AC.alarm1.state == AlarmState::PLAYING || AC.alarm2.state == AlarmState::PLAYING) {
                    ui.transitionToFrame(3); // alarm defuse frame
                } else if (AC.alarm1.state == AlarmState::SNOOZED || AC.alarm2.state == AlarmState::SNOOZED) {
                    ui.transitionToFrame(2); // alarm snooze frame
                } else {
                    AC.uiActive = true;
                    ui.transitionToFrame(4); // info frame
                }
                break;
            case navigation::Direction::Left:
                AC.matrix.scrollPrev();
                matrixScrollTimer.start();
                break;
            case navigation::Direction::Right:
                AC.matrix.scrollNext();
                matrixScrollTimer.start();
                break;
            case navigation::Direction::Up:
                AC.mainLight.incrDuty();
                break;
            case navigation::Direction::Down:
                AC.mainLight.decrDuty();
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm frame
    void uiAlarm(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Left:
            case navigation::Direction::Right:
                ui.transitionToFrame(3); // alarm defuse frame
                break;
            case navigation::Direction::Up:
                AC.mainLight.incrDuty();
                break;
            case navigation::Direction::Down:
                AC.mainLight.decrDuty();
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm snooze frame
    void uiAlarmSnooze(UIDisplay &ui) {
        auto alarmSnz = AC.alarm1.state == AlarmState::SNOOZED || AC.alarm2.state == AlarmState::SNOOZED;
        auto alarmPlaying = AC.alarm1.state == AlarmState::PLAYING || AC.alarm2.state == AlarmState::PLAYING;
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                if (alarmSnz) ui.transitionToFrame(3); // alarm defuse frame
                else {
                    snoozeAlarms();
                    ui.transitionToFrame(0); // home frame
                }
                break;
            case navigation::Direction::Left:
                if (alarmPlaying) ui.transitionToFrame(1); // alarm frame
                else ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Up:
                if (!alarmSnz) {
                    if (AC.snoozeTime < 10) AC.snoozeTime++;
                    else if (AC.snoozeTime < 20) AC.snoozeTime += 5;
                    else AC.snoozeTime += 10;
                    if (AC.snoozeTime > 45) AC.snoozeTime = 2;
                }
                break;
            case navigation::Direction::Down:
                if (!alarmSnz) {
                    if (AC.snoozeTime > 20) AC.snoozeTime -= 10;
                    else if (AC.snoozeTime > 10) AC.snoozeTime -= 5;
                    else if (AC.snoozeTime > 2) AC.snoozeTime--;
                    else AC.snoozeTime = 45;
                }
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm defuse frame
    void uiAlarmDefuse(UIDisplay &ui) {
        static std::array<uint8_t, 6> code{};
        auto cursor = ui.getCursor();
        auto alarmPlaying = AC.alarm1.state == AlarmState::PLAYING || AC.alarm2.state == AlarmState::PLAYING;
        if (cursor == 6) {
            // check if code is correct
            for (uint8_t i = 0; i < 6; ++i) {
                // incorrect code -> go to alarm snooze
                if (code[i] != AC.defuseCode[i]) {
                    AC.snoozeTime = 5; // default snooze time
                    ui.transitionToFrame(2); // alarm snooze frame
                    return;
                }
            }
            // code is correct
            AC.player.stop();
            AC.indicatorLight.toggleOff();
            ui.transitionToFrame(0); // home frame
            stopAlarms();
        }
        switch (getInput()) {
            case navigation::Direction::Center:
                if (alarmPlaying) ui.transitionToFrame(1); // alarm frame
                else ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Left:
                code[cursor] = 0;
                ui.setCursor(cursor + 1);
                break;
            case navigation::Direction::Right:
                code[cursor] = 1;
                ui.setCursor(cursor + 1);
                break;
            case navigation::Direction::Up:
                code[cursor] = 2;
                ui.setCursor(cursor + 1);
                break;
            case navigation::Direction::Down:
                code[cursor] = 3;
                ui.setCursor(cursor + 1);
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the info frame
    void uiOverview(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                ui.transitionToFrame(5); // settings frame
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Up:
            case navigation::Direction::Down:
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the settings frame
    void uiSettings(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                switch (cursor) {
                    case 0:
                        AC.alarmToSet = N::ONE;
                        ui.transitionToFrame(6); // alarm menu frame
                        break;
                    case 1:
                        AC.alarmToSet = N::TWO;
                        ui.transitionToFrame(6); // alarm menu frame
                        break;
                    case 2:
                        ui.transitionToFrame(9); // player menu frame
                        break;
                    case 3:
                        ui.transitionToFrame(13); // light duration frame
                        break;
                    case 4:
                        ui.transitionToFrame(14); // wifi menu frame
                        break;
                    case 5:
                        ui.transitionToFrame(16); // info frame
                        break;
                    default:
                        break;
                }
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(4); // overview frame
                break;
            case navigation::Direction::Up:
                ui.setCursor((cursor + 5) % 6);
                break;
            case navigation::Direction::Down:
                ui.setCursor((cursor + 1) % 6);
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm menu frame
    void uiAlarmMenu(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        auto &alarm = AC.alarmToSet == N::ONE ? AC.alarm1 : AC.alarm2;
        switch (getInput()) {
            case navigation::Direction::Center:
                if (cursor == 1) {
                    alarm.toggle = !alarm.toggle;
                    break;
                } else if (cursor == 2) {
                    setIn8hFromNow(alarm, AC.now);
                    break;
                }
                [[fallthrough]]; // else fall through
            case navigation::Direction::Right:
                switch (cursor) {
                    case 0:
                        ui.transitionToFrame(7); // alarm time frame
                        break;
                    case 1:
                        alarm.toggle = !alarm.toggle;
                        setAlarm(alarm, AC.rtc);
                        ui.transitionToFrame(0); // home frame
                        break;
                    case 2:
                        setIn8hFromNow(alarm, AC.now);
                        ui.transitionToFrame(0); // home frame
                        break;
                    case 3:
                        ui.transitionToFrame(8); // alarm sound frame
                        break;
                    default:
                        break;
                }
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(5); // settings frame
                ui.setCursor(AC.alarmToSet == N::ONE ? 0 : 1);
                break;
            case navigation::Direction::Up:
                ui.setCursor((cursor + 3) % 4);
                break;
            case navigation::Direction::Down:
                ui.setCursor((cursor + 1) % 4);
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm time frame
    void uiAlarmTime(UIDisplay &ui) {
        /*
         * cursor 0: hour tens
         * cursor 1: hour ones
         * cursor 2: minute tens
         * cursor 3: minute ones
         * cursor 4-10: repeat
         */
        auto cursor = ui.getCursor();
        auto &alarm = AC.alarmToSet == N::ONE ? AC.alarm1 : AC.alarm2;
        auto h = (uint8_t) alarm.hour;
        auto m = (uint8_t) alarm.minute;
        auto r = (uint8_t) alarm.repeat;
        switch (getInput()) {
            case navigation::Direction::Left:
                if (cursor == 0) ui.transitionToFrame(6); // alarm menu frame
                else ui.setCursor(cursor - 1);
                break;
            case navigation::Direction::Center:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Right:
                if (cursor == 10) ui.transitionToFrame(0); // home frame
                else ui.setCursor(cursor + 1);
                break;
            case navigation::Direction::Up:
                switch (cursor) {
                    case 0:
                        alarm.hour = (uint8_t) (min(23, (h + 10) % 30));
                        break;
                    case 1:
                        alarm.hour = (uint8_t) (h == 23 ? 20 : h - (h % 10) + (h + 1) % 10);
                        break;
                    case 2:
                        alarm.minute = (uint8_t) (min(59, (m + 10) % 60));
                        break;
                    case 3:
                        alarm.minute = (uint8_t) (min(59, m - (m % 10) + (m + 1) % 10));
                        break;
                    default:
                        alarm.repeat = (uint8_t) ((uint8_t) (r | 1 << (cursor - 4)));
                        break;
                }
                alarm.toggle = true;
                setAlarm(alarm, AC.rtc);
                break;
            case navigation::Direction::Down:
                switch (cursor) {
                    case 0:
                        alarm.hour = (uint8_t) (min(23, (h + 20) % 30));
                        break;
                    case 1:
                        alarm.hour = (uint8_t) (min(23, h - (h % 10) + (h - 1) % 10));
                        break;
                    case 2:
                        alarm.minute = (uint8_t) (min(59, (m + 50) % 60));
                        break;
                    case 3:
                        alarm.minute = (uint8_t) (min(59, m - (m % 10) + (m - 1) % 10));
                        break;
                    default:
                        alarm.repeat = r & ~(1 << (cursor - 4));
                        break;
                }
                alarm.toggle = true;
                setAlarm(alarm, AC.rtc);
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the alarm sound frame
    void uiAlarmSound(UIDisplay &ui) {
        auto numSounds = AC.sounds.size();
        auto &alarm = (AC.alarmToSet == N::ONE ? AC.alarm1 : AC.alarm2);
        auto sound = (uint8_t) alarm.sound;
        switch (getInput()) {
            case navigation::Direction::Center:
                if (sound) AC.player.play(sound);
                else AC.player.play((uint8_t) random((long) numSounds));
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(6); // alarm menu frame
                ui.setCursor(2);
                break;
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Up:
                alarm.sound = (sound + 1) % (numSounds);
                break;
            case navigation::Direction::Down:
                alarm.sound = (uint8_t) ((sound + numSounds - 1) % (numSounds));
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the player menu frame
    void uiPlayerMenu(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        switch (getInput()) {
            case navigation::Direction::Center:
                if (cursor == 2) {
                    AC.player.stop();
                    break;
                }
                [[fallthrough]]; // else fall through
            case navigation::Direction::Right:
                switch (cursor) {
                    case 0:
                        ui.transitionToFrame(10); // player volume frame
                        break;
                    case 1:
                        ui.transitionToFrame(11); // player play frame
                        break;
                    case 2:
                        AC.player.stop();
                        ui.transitionToFrame(0); // home frame
                        break;
                    case 3:
                        ui.transitionToFrame(12); // player sounds frame
                        break;
                    default:
                        break;
                }
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(5); // settings frame
                ui.setCursor(2);
                break;
            case navigation::Direction::Up:
                ui.setCursor((cursor + 3) % 4);
                break;
            case navigation::Direction::Down:
                ui.setCursor((cursor + 1) % 4);
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the player volume frame
    void uiPlayerVolume(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(9); // player menu frame
                break;
            case navigation::Direction::Up:
                AC.player.incrVolume();
                break;
            case navigation::Direction::Down:
                AC.player.decrVolume();
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the player play frame
    void uiPlayerPlay(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        auto numSounds = AC.sounds.size();
        switch (getInput()) {
            case navigation::Direction::Center:
                if (cursor) AC.player.play(cursor);
                else AC.player.play((uint8_t) random((long) numSounds));
                [[fallthrough]]; // fall through
            case navigation::Direction::Left:
                ui.transitionToFrame(9); // player menu frame
                ui.setCursor(1);
                break;
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Up:
                ui.setCursor((cursor + 1) % (numSounds));
                break;
            case navigation::Direction::Down:
                ui.setCursor((uint8_t) ((cursor + numSounds - 1) % (numSounds)));
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the player sounds frame
    void uiPlayerSounds(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        auto numSounds = AC.sounds.size();
        switch (getInput()) {
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                [[fallthrough]]; // fall through
            case navigation::Direction::Center: {
                auto sound = *Sound::getSoundById(cursor + 1, AC.sounds);
                sound.setAllowRandom(!sound.isAllowRandom());
                break;
            }
            case navigation::Direction::Left:
                ui.transitionToFrame(9); // player menu frame
                ui.setCursor(3);
                break;
            case navigation::Direction::Up:
                ui.setCursor((cursor + 1) % (numSounds - 1));
                break;
            case navigation::Direction::Down:
                ui.setCursor((uint8_t) ((cursor + numSounds - 2) % (numSounds - 1)));
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the light duration frame
    void uiLightDuration(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(5); // settings frame
                ui.setCursor(3);
                break;
            case navigation::Direction::Up:
                AC.mainLight.incrDuration();
                break;
            case navigation::Direction::Down:
                AC.mainLight.decrDuration();
                break;
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the Wi-Fi menu frame
    void uiWiFiMenu(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                ui.transitionToFrame(15); // smart config frame
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(5); // settings frame
                ui.setCursor(4);
                break;
            case navigation::Direction::Up:
            case navigation::Direction::Down:
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the smart config frame
    void uiSmartConfig(UIDisplay &ui) {
        auto cursor = ui.getCursor();
        switch (getInput()) {
            case navigation::Direction::Center:
                ui.setCursor(1);
                // run smart config in a new task to not block the ui and the other alarm clock components
                xTaskCreate(
                        [](void *param) {
                            AC.uiTimer.stop();
                            auto *_ui = (UIDisplay *) param;
                            _ui->setCursor(esp32_wifi::smartConfig() ? 3 : 2);
                            AC.uiTimer.start();
                            vTaskDelay(pdMS_TO_TICKS(2500));
                            _ui->setCursor(0);
                            vTaskDelete(nullptr);
                        },
                        "smartConfig",
                        1024,
                        &ui,
                        1,
                        nullptr
                );
                break;
            case navigation::Direction::Right:
                if (cursor != 1) ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Left:
                if (cursor != 1) ui.transitionToFrame(14); // wifi menu frame
                break;
            case navigation::Direction::Up:
            case navigation::Direction::Down:
            case navigation::Direction::None:
                break;
        }
    }

    // ui handle for the info frame
    void uiInfo(UIDisplay &ui) {
        switch (getInput()) {
            case navigation::Direction::Center:
            case navigation::Direction::Right:
                ui.transitionToFrame(0); // home frame
                break;
            case navigation::Direction::Left:
                ui.transitionToFrame(5); // settings frame
                ui.setCursor(5);
                break;
            case navigation::Direction::Up:
            case navigation::Direction::Down:
            case navigation::Direction::None:
                break;
        }
    }

}


#endif //ALARM_CLOCK_UI_HANDLES_H
