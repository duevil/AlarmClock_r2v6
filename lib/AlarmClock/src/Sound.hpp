//
// Created by Malte on 06.10.2023.
//

#ifndef ALARM_CLOCK_SOUND_H
#define ALARM_CLOCK_SOUND_H


namespace AlarmClock {

    /**
     * @brief Class that represents a sound
     */
    class Sound {

        uint8_t id;
        String name;
        bool allowRandom;
        bool played{false};

    public:

        Sound(uint8_t id, String name, bool allowRandom) : id(id), name(std::move(name)), allowRandom(allowRandom) {}

        uint8_t getId() const { return id; }

        String getName() const { return name; }

        bool isAllowRandom() const { return allowRandom; }

        void setAllowRandom(bool value) { allowRandom = value; }

        /**
         * @brief Creates JSON data from the sound
         * @param json The JSON object to create the JSON data in
         */
        void toJson(JsonVariant &json) const {
            json["id"] = id;
            json["name"] = name;
            json["allowRandom"] = allowRandom;
        }

        /**
         * @brief Creates a sound from the given JSON data
         * @param json The JSON data to create the sound from
         * @return The sound created from the given JSON data
         */
        static Sound fromJson(JsonVariant &json) {
            return {json["id"], json["name"], json["allowRandom"]};
        }

        /**
         * @brief Creates JSON data from the given sounds
         * @param sounds The sounds to create JSON data from
         * @param json The JSON object to create the JSON data in
         */
        static void soundsToJson(std::vector<Sound> &sounds, JsonVariant &json) {
            for (auto &sound: sounds) {
                JsonVariant soundJson = json.createNestedObject();
                soundJson["id"] = sound.id;
                soundJson["name"] = sound.name;
                soundJson["allowRandom"] = sound.allowRandom;
            }
        }

        /**
         * @brief Creates a vector of sounds from the given JSON data
         * @param json The JSON data to create the sounds from
         * @return The sounds created from the given JSON data
         */
        static std::vector<Sound> soundsFromJson(JsonVariant &json) {
            std::vector<Sound> sounds;
            auto array = json.as<JsonArray>();
            for (auto soundJson: array) {
                sounds.emplace_back(soundJson["id"], soundJson["name"], soundJson["allowRandom"]);
            }
            return sounds;
        }

        /**
         * @brief Loads the sounds from the given file on the SPIFFS filesystem as JSON data
         * @param fileName The name of the file to load the sounds from
         * @return The sounds loaded from the given file
         */
        static std::vector<Sound> loadSounds(const char *fileName) {
            auto file = SPIFFS.open(fileName);
            if (!file) return {};
            DynamicJsonDocument doc(JSON_SOUNDS_BUF_SIZE);
            auto error = deserializeJson(doc, file);
            assert(!error && "Failed to parse sounds file");
            file.close();
            JsonVariant json = doc.as<JsonVariant>();
            return soundsFromJson(json);
        }

        /**
         * @brief Saves the given sounds as JSON data to the given file on the SPIFFS filesystem
         * @param fileName The name of the file to save the sounds to
         * @param sounds The sounds to save
         */
        static void saveSounds(const char *fileName, std::vector<Sound> &sounds) {
            auto file = SPIFFS.open(fileName, FILE_WRITE);
            assert(file && "Failed to open sounds file");
            JsonVariant json = DynamicJsonDocument(JSON_SOUNDS_BUF_SIZE);
            soundsToJson(sounds, json);
            auto error = serializeJson(json, file);
            assert(!error && "Failed to serialize sounds file");
            file.close();
        }

        /**
         * @brief Returns an iterator to the sound with the given id
         * @param id The id of the sound to search for
         * @param sounds The sounds to search in
         * @return An iterator to the sound with the given id
         */
        static __gnu_cxx::__normal_iterator<Sound *, std::vector<Sound>>
        getSoundById(uint8_t id, std::vector<Sound> &sounds) {
            auto sound = std::find_if(
                    sounds.begin(),
                    sounds.end(),
                    [id](const Sound &s) { return s.getId() == id; }
            );
            return sound;
        }

        /**
         * @brief Finds a random sound that is allowed to be played randomly and has not been played yet\n
         * If no such sound exists, all sounds that are allowed to be played randomly are reset and a random sound is
         * returned. The sound that is returned is marked as played.
         * @param sounds The sounds to search in
         * @return A random sound that is allowed to be played randomly and has not been played yet
         */
        static const Sound &findRandomSound(std::vector<Sound> &sounds) {
            std::vector<int> validIndices;
            for (int i = 0; i < sounds.size(); ++i) {
                if (sounds[i].allowRandom && !sounds[i].played) {
                    validIndices.push_back(i);
                }
            }

            if (validIndices.empty()) {
                for (int i = 0; i < sounds.size(); ++i) {
                    if (sounds[i].allowRandom) {
                        sounds[i].played = false;
                        validIndices.push_back(i);
                    }
                }
            }

            auto randomIndex = random((long) validIndices.size());
            auto selectedSoundIndex = validIndices[randomIndex];
            sounds[selectedSoundIndex].played = true;
            return sounds[selectedSoundIndex];
        }

    };

}


#endif //ALARM_CLOCK_SOUND_H
