//
// Created by Malte on 05.10.2023.
//

#ifndef ALARM_CLOCK_PLAYER_H
#define ALARM_CLOCK_PLAYER_H

// TODO:
//  maybe add file count reading to allow alarm having sounds
//  when no sound names were stored on the internal storage


namespace AlarmClock {

    /**
     * @brief The player class that handles the DFPlayer Mini and contains the player's current volume
     */
    class Player {

        DFRobotDFPlayerMini player{};
        Uint8Bean volume;

    public:

        explicit Player(Preferences &preferences) : volume("volume", preferences) {}

        /**
         * @brief Sets up the connection to the DFPlayer Mini; does not reset the DFPlayer device
         * @return true if setup was successful, false otherwise
         */
        bool setup() {
            Serial2.begin(9600);
            if (player.begin(Serial2, true, false)) {
                player.setTimeOut(500);
                player.EQ(DFPLAYER_EQ_NORMAL);
                volume.load();
                player.volume((uint8_t) volume);
                player.outputDevice(DFPLAYER_DEVICE_SD);
                return true;
            }
            return false;
        }

        /**
         * @brief Returns the current volume
         * @return The current volume
         */
        uint8_t getVolume() {
            // readVolume() is not working
            // auto v = (uint8_t) player.readVolume();
            // assert(v == (uint8_t) volume && "Volume mismatch");
            return (uint8_t) volume;
        }

        /**
         * @brief Sets the volume to the given value
         * @param v The volume to set; is clamped to 0-30
         */
        void setVolume(uint8_t v) { player.volume(min((uint8_t) (volume = v), (uint8_t) 30)); }

        /**
         * @brief Increases the volume by 1
         */
        void incrVolume() { setVolume((getVolume() + 1) % 31); }

        /**
         * @brief Decreases the volume by 1
         */
        void decrVolume() { setVolume((getVolume() + 30) % 31); }

        /**
         * @brief Plays the given sound
         * @param sound The sound to play; if 0, sound 1 is played
         */
        void play(uint8_t sound) { player.play(max(sound, (uint8_t) 1)); }

        /**
         * @brief Plays the given sound in a loop
         * @param sound The sound to play; if 0, sound 1 is played
         */
        void playLoop(uint8_t sound) { player.loop(max(sound, (uint8_t) 1)); }

        /**
         * @brief Stops the player playback
         */
        void stop() { player.stop(); }

        // delete copy constructor and assignment operator

        Player(const Player &) = delete;

        Player &operator=(const MainLight &) = delete;

    };

}


#endif //ALARM_CLOCK_PLAYER_H
