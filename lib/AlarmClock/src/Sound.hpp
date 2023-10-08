//
// Created by Malte on 06.10.2023.
//

#ifndef ALARM_CLOCK_SOUND_H
#define ALARM_CLOCK_SOUND_H


namespace AlarmClock {

    class Sound {

        uint8_t id;
        String name;
        bool allowRandom;
        bool played{false};

    public:

        Sound(uint8_t id, String name, bool allowRandom) : id(id), name(std::move(name)), allowRandom(allowRandom) {}

        uint8_t getId() const { return id; }

        String getName() const { return name; }

        void toJson(JsonVariant &json) const {
            json["id"] = id;
            json["name"] = name;
            json["allowRandom"] = allowRandom;
        }

        static Sound fromJson(JsonVariant &json) {
            return {json["id"], json["name"], json["allowRandom"]};
        }

        static void soundsToJson(std::vector<Sound> &sounds, JsonVariant &json) {
            for (auto &sound: sounds) {
                JsonVariant soundJson = json.createNestedObject();
                soundJson["id"] = sound.id;
                soundJson["name"] = sound.name;
                soundJson["allowRandom"] = sound.allowRandom;
            }
        }

        static std::vector<Sound> soundsFromJson(JsonVariant &json) {
            std::vector<Sound> sounds;
            auto array = json.as<JsonArray>();
            for (auto soundJson: array) {
                sounds.emplace_back(soundJson["id"], soundJson["name"], soundJson["allowRandom"]);
            }
            return sounds;
        }

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

        static void saveSounds(const char *fileName, std::vector<Sound> &sounds) {
            auto file = SPIFFS.open(fileName, FILE_WRITE);
            assert(file && "Failed to open sounds file");
            JsonVariant json = DynamicJsonDocument(JSON_SOUNDS_BUF_SIZE);
            soundsToJson(sounds, json);
            auto error = serializeJson(json, file);
            assert(!error && "Failed to serialize sounds file");
            file.close();
        }

        static __gnu_cxx::__normal_iterator<Sound *, std::vector<Sound>>
        getSoundById(uint8_t id, std::vector<Sound> &sounds) {
            auto sound = std::find_if(
                    sounds.begin(),
                    sounds.end(),
                    [id](const Sound &s) { return s.getId() == id; }
            );
            return sound;
        }

        static const Sound &findRandomSound(std::vector<Sound> &sounds) {
            std::random_device rd;
            std::mt19937 gen(rd());

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

            std::uniform_int_distribution<int> dist(0, (int) validIndices.size() - 1);
            auto randomIndex = dist(gen);

            auto selectedSoundIndex = validIndices[randomIndex];
            sounds[selectedSoundIndex].played = true;
            return sounds[selectedSoundIndex];
        }

    };

}


#endif //ALARM_CLOCK_SOUND_H
