//
// Created by Malte on 05.10.2023.
//

#ifndef ALARM_CLOCK_PLAYER_H
#define ALARM_CLOCK_PLAYER_H


namespace AlarmClock {
    namespace player {

        void setup(DFRobotDFPlayerMini &player = AC.player) {
            Serial2.begin(9600);
            assert(player.begin(Serial2, false, false));
            player.setTimeOut(500);
            player.EQ(DFPLAYER_EQ_NORMAL);
            player.outputDevice(DFPLAYER_DEVICE_SD);
        }

    }
}


#endif //ALARM_CLOCK_PLAYER_H
