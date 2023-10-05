//
// Created by Malte on 04.10.2023.
//

#ifndef ALARM_CLOCK_WEBSERVER_H
#define ALARM_CLOCK_WEBSERVER_H

namespace AlarmClock {
    namespace webserver {

        void setup(AsyncWebServer &server = AC.server) {
            assert(SPIFFS.begin() && "SPIFFS failed to mount");
            server.serveStatic("/", SPIFFS, "/root_site/").setDefaultFile("index.html");
            server.begin();
            AsyncElegantOTA.setID("AlarmClock");
            AsyncElegantOTA.begin(&server);
        }

        // TODO: API endpoints

    }
}

#endif //ALARM_CLOCK_WEBSERVER_H
