//
// Created by Malte on 05.10.2023.
//

#ifndef ALARM_CLOCK_PLAYER_H
#define ALARM_CLOCK_PLAYER_H


namespace AlarmClock {

    class Player {

        DFRobotDFPlayerMini player{};
        Uint8Bean volume;

    public:

        explicit Player(Preferences &preferences) : volume("volume", preferences) {}

        void setup() {
            Serial2.begin(9600);
            assert(player.begin(Serial2, false, false));
            player.setTimeOut(500);
            player.EQ(DFPLAYER_EQ_NORMAL);
            volume.load();
            player.volume((uint8_t) volume);
            player.outputDevice(DFPLAYER_DEVICE_SD);
        }

        uint8_t getVolume() {
            auto v = (uint8_t) player.readVolume();
            assert(v == (uint8_t) volume && "Volume mismatch");
            return v;
        }

        void setVolume(uint8_t v) { player.volume(min((uint8_t)(volume = v), (uint8_t) 30)); }

        void play(uint8_t sound) { player.play(max(sound, (uint8_t) 1)); }

        void stop() { player.stop(); }

    };

}


#endif //ALARM_CLOCK_PLAYER_H