//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_WEBSERVER_H
#define ALARM_CLOCK_WEBSERVER_H

// TODO: test API endpoints


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
            request->send(200, "text/plain", String(millis()));
        }

        void getLightSensor(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root["value"] = AC.lightLevel;
            response->setLength();
            request->send(response);
        }

        void play(AsyncWebServerRequest * request) {
            if (request->hasParam("sound")) {
                auto sound = request->getParam("sound")->value().toInt();
                // TODO: check if sound is valid
                // TODO: play sound
                if (sound == 0) AC.player.play(1);
                else AC.player.play(sound);
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void stop(AsyncWebServerRequest * request) {
            // TODO: stop sound
            AC.player.stop();
            request->send(204);
        }

        void putTimeZone(AsyncWebServerRequest *request, JsonVariant &json) {
            auto timeZone = json["timeZone"].as<const char *>();
            if (setenv("TZ", timeZone, 1) == 0) {
                tzset();
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
                if (id != 1 && id != 2) {
                    request->send(404, "text/plain", "Invalid alarm id");
                    return;
                }
                auto *response = new AsyncJsonResponse();
                auto root = response->getRoot();
                // TODO: alarm data
                root["id"] = id;
                root["hour"] = 0;
                root["minute"] = 0;
                root["repeat"] = 0;
                root["toggle"] = false;
                root["sound"] = 0;
                root["nextDateTime"] = 0;
                response->setLength();
                request->send(response);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void getLight(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root["duty"] = AC.mainLight.getDuty();
            root["duration"] = AC.mainLightDuration.get();
            response->setLength();
            request->send(response);
        }

        void getPlayer(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            // TODO: player data
            root["volume"] = AC.player.readVolume();
            response->setLength();
            request->send(response);
        }

        void getSounds(AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            auto root = response->getRoot();
            root.createNestedArray();
            // TODO: sound data
            response->setLength();
            request->send(response);
        }

        void getSound(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = request->getParam("id")->value().toInt();
                // TODO: check if id is valid
                auto *response = new AsyncJsonResponse();
                auto root = response->getRoot();
                // TODO: sound data
                root["id"] = id;
                root["name"] = "na";
                root["allowRandom"] = false;
                response->setLength();
                request->send(response);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        //#endregion
        //#region data PUT, POST, DELETE

        void putAlarm(AsyncWebServerRequest *request, JsonVariant &json) {
            auto id = json["id"].as<uint8_t>();
            if (id != 1 && id != 2) {
                request->send(404, "text/plain", "Invalid alarm id");
                return;
            }
            // TODO: alarm data
            request->send(204);
        }

        void putAlarmIn8h(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = request->getParam("id")->value().toInt();
                if (id != 1 && id != 2) {
                    request->send(404, "text/plain", "Invalid alarm id");
                    return;
                }
                // TODO: set alarm in 8h
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        void putLight(AsyncWebServerRequest *request, JsonVariant &json) {
            AC.mainLight.setDuty(json["duty"].as<uint8_t>());
            AC.mainLightDuration = json["duration"].as<uint8_t>();
            if (AC.mainLight.getDuty()) AC.mainLightTimer.start();
            request->send(204);
        }

        void putPlayer(AsyncWebServerRequest *request, JsonVariant &json) {
            // TODO: player data
            auto volume = json["volume"].as<uint8_t>();
            if (volume > 30) {
                request->send(400, "text/plain", "Invalid must be between 0 and 30");
                return;
            }
            AC.player.volume(volume);
            request->send(204);
        }

        void putSounds(AsyncWebServerRequest *request, JsonVariant &json) {
            // TODO: sounds data
            request->send(204);
        }

        void putSound(AsyncWebServerRequest *request, JsonVariant &json) {
            auto id = json["id"].as<uint8_t>();
            // TODO: check if id is valid
            // TODO: sound data
            request->send(204);
        }

        void postSound(AsyncWebServerRequest *request, JsonVariant &json) {
            // TODO: sound data
            request->send(204);
        }

        void deleteSound(AsyncWebServerRequest *request) {
            if (request->hasParam("id")) {
                auto id = request->getParam("id")->value().toInt();
                // TODO: check if id is valid
                // TODO: delete sound
                request->send(204);
            } else {
                request->send(400, "text/plain", "Missing parameter");
            }
        }

        //#endregion

        void setup(AsyncWebServer &server) {
            assert(SPIFFS.begin() && "SPIFFS failed to mount");
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
            server.on("/alarm_in_8h", HTTP_PUT, putAlarmIn8h);
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
