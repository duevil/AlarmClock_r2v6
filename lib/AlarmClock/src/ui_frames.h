//
// Created by Malte on 09.10.2023.
//

#ifndef ALARM_CLOCK_UI_FRAMES_H
#define ALARM_CLOCK_UI_FRAMES_H


namespace AlarmClock {

    void home(UIGraphics) { /* empty */ }

    void alarm(UIGraphics ui) {
        static uint32_t timer = 0;
        static uint8_t z = 0;
        if (millis() - timer > 500) {
            timer = millis();
            z = z == 3 ? 0 : z + 1;
        }
        switch (z) {
            case 1:
                ui.drawBitMap(10, 0, bitmaps::alarmClockRing);
                break;
            case 3:
                ui.drawBitMap(-10, 0, bitmaps::alarmClockRing);
                break;
            default:
                ui.drawBitMap(0, 0, bitmaps::alarmClock);
                break;
        }
    }

    void snoozeAlarm(UIGraphics ui) {
        if (AC.alarm1.state == AlarmState::SNOOZED || AC.alarm2.state == AlarmState::SNOOZED) {
            ui.drawTitle("Alarm Snoozed");
            ui.drawLine(UserInterface::Line::L3, "STOP >", TEXT_ALIGN_CENTER);
        } else {
            ui.drawTitle("Snooze Alarm");
            ui.drawSetter(UserInterface::Line::L2, "Time", AC.snoozeTime, "min");
        }
    }

    void defuseAlarm(UIGraphics ui) {
        ui.drawTitle("Defuse Alarm");
        for (int i = 0; i < 6; ++i) {
            std::array<const UserInterface::BitMap *, 4> arrow = {
                    &bitmaps::arrowLeft,
                    &bitmaps::arrowRight,
                    &bitmaps::arrowUp,
                    &bitmaps::arrowDown,
            };
            ui.drawBitMap((int16_t) (14 + i * 18), 28, *arrow[AC.defuseCode[i]]);
        }
        ui.display->setTextAlignment(TEXT_ALIGN_LEFT);
        ui.display->drawString((int16_t) (13 + ui.cursor() * 18 + ui.x), (int16_t) (30 + ui.y), "__");
    }

    void overview(UIGraphics ui) {
        String buf("DDD, DD. MMM 'YY");
        AC.now.toString(buf.begin());
        ui.drawLine(UserInterface::Line::L1, buf, TEXT_ALIGN_CENTER);
        auto a1dt = getAlarmTime(AC.alarm1, AC.now);
        auto a2dt = getAlarmTime(AC.alarm2, AC.now);
        DateTime nil{};
        auto next = 0;
        if (a1dt != nil) {
            if (a2dt != nil) next = a1dt < a2dt ? 1 : 2;
            else next = 1;
        } else if (a2dt != nil) next = 2;
        if (next != 0) {
            auto ts = (next == 1 ? a1dt : a2dt) - AC.now;
            sprintf(buf.begin(), "A%u in %ud %uh %um %us", next, ts.days(), ts.hours(), ts.minutes(), ts.seconds());
            ui.drawLine(UserInterface::Line::L2, buf);
        } else ui.drawLine(UserInterface::Line::L2, "no alarm set");
        if (AC.alarm1.toggle) {
            buf = "A1: DDD, DD.MM. hh:mm";
            a1dt.toString(buf.begin());
            ui.drawLine(UserInterface::Line::L4, buf);
        } else ui.drawLine(UserInterface::Line::L4, "A1: off");
        if (AC.alarm2.toggle) {
            buf = "A2: DDD, DD.MM. hh:mm";
            a2dt.toString(buf.begin());
            ui.drawLine(UserInterface::Line::L5, buf);
        } else ui.drawLine(UserInterface::Line::L5, "A2: off");
    }

    void settings(UIGraphics ui) {
        ui.drawOptions("Settings",
                       "Set Alarm 1",
                       "Set Alarm 2",
                       "Player Menu",
                       "Light Duration",
                       "WiFi Menu",
                       "Information");
    }

    void alarmMenu(UIGraphics ui) {
        auto title = AC.alarmToSet == N::ONE ? "Alarm 1 Menu" : "Alarm 2 Menu";
        auto toggle = (AC.alarmToSet == N::ONE ? AC.alarm1 : AC.alarm2).toggle ? "Disable" : "Enable";
        ui.drawOptions(title, "Set time", toggle, "Set in 8h", "Set sound");
    }

    void alarmTime(UIGraphics ui) {
        auto title = AC.alarmToSet == N::ONE ? "Set Alarm 1 Time" : "Set Alarm 2 Time";
        auto &alarm = AC.alarmToSet == N::ONE ? AC.alarm1 : AC.alarm2;
        auto cursor = ui.cursor();
        ui.drawTitle(title);
        ui.display->setTextAlignment(TEXT_ALIGN_LEFT);
        auto xPos = cursor < 2 ? 8 : 9;
        xPos = cursor < 4 ? (cursor * 6 + 6 * (xPos)) : ((cursor - 4) * 6 + 6 * 10);
        auto yPos = cursor < 4 ? 16 : 30;
        ui.display->drawString((int16_t) (xPos + ui.x), (int16_t) (yPos + ui.y), cursor <= 10 ? "_" : "");
        String buf{};
        buf.reserve(24);
        ui.display->drawStringf(
                ui.x, (int16_t) (14 + ui.y), buf.begin(), "%c Time: %02u:%02u",
                cursor < 4 ? '>' : ' ',
                (uint8_t) alarm.hour, (uint8_t) alarm.minute
        );
        String days = "smtwtfs";
        for (uint8_t i = 0; i < 7; i++) if ((uint8_t) alarm.repeat & (1 << i)) days[i] -= 32;
        ui.display->drawStringf(
                ui.x, (int16_t) (28 + ui.y), buf.begin(), "%c Repeat: " + days,
                cursor >= 4 ? '>' : ' '
        );
    }

    void alarmSound(UIGraphics ui) {
        auto title = AC.alarmToSet == N::ONE ? "Set Alarm 1 Sound" : "Set Alarm 2 Sound";
        ui.drawTitle(title);
        auto soundID = (uint8_t) (AC.alarmToSet == N::ONE ? AC.alarm1.sound : AC.alarm2.sound);
        ui.drawSetter(UserInterface::Line::L2, "Sound #", soundID);
        if (soundID) {
            auto &sound = *Sound::getSoundById(soundID, AC.sounds);
            ui.drawLine(UserInterface::Line::L3, sound.getName());
        } else ui.drawLine(UserInterface::Line::L3, "~ random sound");
        ui.drawLine(UserInterface::Line::L5, "Press MID t0 preview", TEXT_ALIGN_CENTER);
    }

    void playerMenu(UIGraphics ui) {
        ui.drawOptions("Player Menu",
                       "Set volume",
                       "Play sound",
                       "Stop playback",
                       "Edit sounds");
    }

    void playerVolume(UIGraphics ui) {
        ui.drawTitle("Player Volume");
        ui.drawSetter(UserInterface::Line::L2,
                      "Volume",
                      (uint8_t) ((int) AC.player.getVolume() * 100 / 30),
                      "%");
    }

    void playerPlay(UIGraphics ui) {
        ui.drawTitle("Play Sound");
        ui.drawSetter(UserInterface::Line::L2, "Sound #", ui.cursor());
        if (ui.cursor()) {
            auto &sound = *Sound::getSoundById(ui.cursor(), AC.sounds);
            ui.drawLine(UserInterface::Line::L3, sound.getName());
        } else ui.drawLine(UserInterface::Line::L3, "~ random sound");
    }

    void playerSounds(UIGraphics ui) {
        ui.drawTitle("Set sound random play");
        ui.drawSetter(UserInterface::Line::L2, "Sound #", ui.cursor() + 1);
        auto &sound = *Sound::getSoundById(ui.cursor() + 1, AC.sounds);
        ui.drawLine(UserInterface::Line::L3, sound.getName());
        ui.drawLine(UserInterface::Line::L4, sound.isAllowRandom() ? "random = true" : "random = false");
    }

    void lightDuration(UIGraphics ui) {
        ui.drawTitle("Set light duration");
        ui.drawSetter(UserInterface::Line::L2, "Duration", AC.mainLight.getDuration(), "min");
        ui.drawLine(UserInterface::Line::L4, "The duration the");
        ui.drawLine(UserInterface::Line::L5, "main light remains on");
    }

    void wifiMenu(UIGraphics ui) {
        ui.drawOptions("WiFi Menu", "SmartConfig");
        ui.drawLine(UserInterface::Line::L5,
                    !esp32_wifi::isConnected() ? "WiFi not connected" : "IP: " + esp32_wifi::getIP(),
                    TEXT_ALIGN_CENTER);
    }

    void smartConfig(UIGraphics ui) {
        ui.drawTitle("SmartConfig");
        ui.drawLine(UserInterface::Line::L2, "Press MID to start", TEXT_ALIGN_CENTER);
        switch (ui.cursor()) {
            default:
                ui.drawLine(UserInterface::Line::L4, "Not running", TEXT_ALIGN_CENTER);
                break;
            case 1:
                ui.drawLine(UserInterface::Line::L4, "Running...", TEXT_ALIGN_CENTER);
                break;
            case 2:
                ui.drawLine(UserInterface::Line::L4, "Failed!", TEXT_ALIGN_CENTER);
                break;
            case 3:
                ui.drawLine(UserInterface::Line::L4, "Success.", TEXT_ALIGN_CENTER);
                break;
        }
    }

    void info(UIGraphics ui) {
        ui.drawTitle("Information");
        ui.drawLine(UserInterface::Line::L2, "Light:");
        String buf{};
        buf.reserve(20);
        sprintf(buf.begin(), "%.3flx", AC.lightLevel);
        ui.drawLine(UserInterface::Line::L2, buf, TEXT_ALIGN_RIGHT);
        ui.drawLine(UserInterface::Line::L3, "Runtime:");
        auto rt = millis() / 1000;
        TimeSpan ts{static_cast<int32_t>(rt)};
        sprintf(buf.begin(), "%ud%uh%um%us", ts.days(), ts.hours(), ts.minutes(), ts.seconds());
        ui.drawLine(UserInterface::Line::L3, buf, TEXT_ALIGN_RIGHT);
        ui.drawLine(UserInterface::Line::L5, "(c) Malte Kasolowsky", TEXT_ALIGN_CENTER);
    }

}


#endif //ALARM_CLOCK_UI_FRAMES_H
