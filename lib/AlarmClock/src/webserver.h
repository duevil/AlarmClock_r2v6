//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_WEBSERVER_H
#define ALARM_CLOCK_WEBSERVER_H


namespace AlarmClock {
    namespace webserver {

        void setup(AsyncWebServer & = AC.server);

        //#region general GET

        void getCurrentDateTime(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root["value"] = AC.now.unixtime();
            response->setLength();
            request->send(response);
        }

        void getOnTime(AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(millis() / 1000));
        }

        void getLightSensor(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root["value"] = AC.lightLevel;
            response->setLength();
            request->send(response);
        }

        void play(AsyncWebServerRequest *request) {
            if (request->hasParam("sound")) {
                auto sound = (uint8_t) request->getParam("sound")->value().toInt();
                if (sound == 0) {
                    auto rnd = (uint8_t) random(1, (long) AC.sounds.size() + 1);
                    AC.player.play(rnd);
                } else {
                    auto itr = std::find_if(
                            AC.sounds.begin(),
                            AC.sounds.end(),
                            [&sound](const Sound &s) { return s.getId() == sound; }
                    );
                    if (itr != AC.sounds.end()) {
                        AC.player.play(sound);
                    } else {
                        request->send(404, "text/plain", "Sound not found");
                        return;
                    }
                }
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void stop(AsyncWebServerRequest *request) {
            AC.player.stop();
            request->send(204);
        }

        void putTimeZone(AsyncWebServerRequest *request, JsonVariant &json) {
            auto timeZone = json["timeZone"].as<const char *>();
            if (setenv("TZ", timeZone, 1) == 0) {
                tzset();
                AC.tz = timeZone;
                request->send(204);
            } else {
                request->send(400, "text/plain", "Invalid time zone");
            }
        }

        //#endregion
        //#region data GET

        void getAlarm(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = request->getParam("id")->value().toInt();
                auto *response = new AsyncJsonResponse();
                auto root = response->getRoot();
                switch (id) {
                    case 1:
                        alarmToJson(AC.alarm1, root, AC.now);
                        break;
                    case 2:
                        alarmToJson(AC.alarm2, root, AC.now);
                        break;
                    default:
                        request->send(404, "text/plain", "Invalid alarm id");
                        return;
                }
                response->setLength();
                request->send(response);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void getLight(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            AC.mainLight.toJson(root);
            response->setLength();
            request->send(response);
        }

        void getPlayer(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root["volume"] = AC.player.getVolume();
            response->setLength();
            request->send(response);
        }

        void getSounds(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse(true, JSON_SOUNDS_BUF_SIZE);
            auto root = response->getRoot();
            Sound::soundsToJson(AC.sounds, root);
            response->setLength();
            request->send(response);
        }

        void getSound(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = (uint8_t) request->getParam("id")->value().toInt();
                auto sPtr = Sound::getSoundById(id, AC.sounds);
                if (sPtr != AC.sounds.end()) {
                    auto *response = new AsyncJsonResponse();
                    auto root = response->getRoot();
                    sPtr->toJson(root);
                    response->setLength();
                    request->send(response);
                } else {
                    request->send(404, "text/plain", "Sound not found");
                }
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        //#endregion
        //#region data PUT, POST, DELETE

        void putAlarm(AsyncWebServerRequest *request, JsonVariant &json) {
            auto id = json["id"].as<uint8_t>();
            switch (id) {
                case 1:
                    alarmFromJson(AC.alarm1, json);
                    setAlarm(AC.alarm1, AC.rtc);
                    break;
                case 2:
                    alarmFromJson(AC.alarm2, json);
                    setAlarm(AC.alarm2, AC.rtc);
                    break;
                default:
                    request->send(404, "text/plain", "Invalid alarm id");
                    return;
            }
            request->send(204);
        }

        void putAlarmIn8h(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = request->getParam("id")->value().toInt();
                switch (id) {
                    case 1:
                        setIn8hFromNow(AC.alarm1, AC.now);
                        break;
                    case 2:
                        setIn8hFromNow(AC.alarm2, AC.now);
                        break;
                    default:
                        request->send(404, "text/plain", "Invalid alarm id");
                        return;
                }
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void putLight(AsyncWebServerRequest *request, JsonVariant &json) {
            AC.mainLight.fromJson(json);
            request->send(204);
        }

        void putPlayer(AsyncWebServerRequest *request, JsonVariant &json) {
            auto volume = json["volume"].as<uint8_t>();
            if (volume > 30) {
                request->send(400, "text/plain", "Invalid must, be between 0 and 30");
                return;
            }
            AC.player.setVolume(volume);
            request->send(204);
        }

        void putSounds(AsyncWebServerRequest *request, JsonVariant &json) {
            AC.sounds = Sound::soundsFromJson(json);
            Sound::saveSounds(JSON_SOUNDS_FILE_NAME, AC.sounds);
            request->send(204);
        }

        void putSound(AsyncWebServerRequest *request, JsonVariant &json) {
            auto sound = Sound::fromJson(json);
            auto sPtr = Sound::getSoundById(sound.getId(), AC.sounds);
            if (sPtr != AC.sounds.end()) {
                *sPtr = sound;
                Sound::saveSounds(JSON_SOUNDS_FILE_NAME, AC.sounds);
                request->send(204);
            } else {
                request->send(404, "text/plain", "Sound not found");
            }
        }

        void postSound(AsyncWebServerRequest *request, JsonVariant &json) {
            auto sound = Sound::fromJson(json);
            if (sound.getId() != AC.sounds.size() + 1) {
                request->send(400, "text/plain", "Invalid sound id");
                return;
            }
            AC.sounds.push_back(sound);
            request->send(204);
        }

        void deleteSound(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = (uint8_t) request->getParam("id")->value().toInt();
                auto sPtr = Sound::getSoundById(id, AC.sounds);
                if (sPtr != AC.sounds.end()) {
                    AC.sounds.erase(sPtr);
                    Sound::saveSounds(JSON_SOUNDS_FILE_NAME, AC.sounds);
                }
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        //#endregion

        void setup(AsyncWebServer &server) {
            server.serveStatic("/", SPIFFS, "/root_site/").setDefaultFile("index.html");

            // general GET

            server.on("/current_datetime", HTTP_GET, getCurrentDateTime);
            server.on("/on_time", HTTP_GET, getOnTime);
            server.on("/light_sensor", HTTP_GET, getLightSensor);
            server.on("/play", HTTP_GET, play);
            server.on("/stop", HTTP_GET, stop);
            auto putTimeZoneHandler = new AsyncCallbackJsonWebHandler("/time_zone", putTimeZone);
            putTimeZoneHandler->setMethod(HTTP_PUT);
            server.addHandler(putTimeZoneHandler);

            // data GET

            server.on("/alarm", HTTP_GET, getAlarm);
            server.on("/light", HTTP_GET, getLight);
            server.on("/player", HTTP_GET, getPlayer);
            server.on("/sounds", HTTP_GET, getSounds);
            server.on("/sound", HTTP_GET, getSound);

            // data PUT, POST, DELETE

            auto putAlarmHandler = new AsyncCallbackJsonWebHandler("/alarm", putAlarm);
            putAlarmHandler->setMethod(HTTP_PUT);
            server.addHandler(putAlarmHandler);
            server.on("/alarm/in8h", HTTP_PUT, putAlarmIn8h);
            auto putLightHandler = new AsyncCallbackJsonWebHandler("/light", putLight);
            putLightHandler->setMethod(HTTP_PUT);
            server.addHandler(putLightHandler);
            auto putPlayerHandler = new AsyncCallbackJsonWebHandler("/player", putPlayer);
            putPlayerHandler->setMethod(HTTP_PUT);
            server.addHandler(putPlayerHandler);
            auto putSoundsHandler = new AsyncCallbackJsonWebHandler("/sounds", putSounds);
            putSoundsHandler->setMethod(HTTP_PUT);
            server.addHandler(putSoundsHandler);
            auto putSoundHandler = new AsyncCallbackJsonWebHandler("/sound", putSound);
            putSoundHandler->setMethod(HTTP_PUT);
            server.addHandler(putSoundHandler);
            auto postSoundHandler = new AsyncCallbackJsonWebHandler("/sound", postSound);
            postSoundHandler->setMethod(HTTP_POST);
            server.addHandler(postSoundHandler);
            server.on("/sound", HTTP_DELETE, deleteSound);


            server.begin();
            AsyncElegantOTA.setID("AlarmClock");
            AsyncElegantOTA.begin(&server);
        }

    }
}


#endif //ALARM_CLOCK_WEBSERVER_H
