#include <utility>

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
            serializeJson(json, Serial);
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
            std::vector<Sound> entries{};
            std::copy_if(
                    sounds.begin(),
                    sounds.end(),
                    std::back_inserter(entries),
                    [](const Sound &sound) { return sound.allowRandom && !sound.played; }
            );
            if (entries.empty()) {
                std::for_each(sounds.begin(), sounds.end(), [](Sound &sound) { sound.played = false; });
                std::copy_if(
                        sounds.begin(),
                        sounds.end(),
                        std::back_inserter(entries),
                        [](const Sound &sound) { return sound.allowRandom && !sound.played; }
                );
            }
            std::random_device rd{};
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, (int) entries.size() - 1);
            auto selected = std::find_if(
                    sounds.begin(),
                    sounds.end(),
                    [&entries, &dis, &gen](Sound &sound) {
                        return sound.getId() == entries[dis(gen)].getId();
                    }
            );
            assert(selected != sounds.end() && "No random sound found");
            selected->played = true;
        }

    };

}


#endif //ALARM_CLOCK_SOUND_H
