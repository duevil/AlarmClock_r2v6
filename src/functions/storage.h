//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_STORAGE_H
#define ALARM_CLOCK_R2V6_STORAGE_H

#include <Arduino.h>
#include <Preferences.h>
#include <map>
#include "classes/Property.hpp"

namespace storage {

    /**
     * @brief Initialises the internal Preferences storage with the given name and syncs it with the given properties;
     * if a property already exists in the storage, it's value will be loaded, else if will be newly assigned
     * @param name The name of the Preferences partition
     * @param properties The properties which will be synced
     */
    void init(const char *name, const std::map<const char *, Property<uint8_t> *>* properties) {
        static Preferences preferences;
        preferences.begin(name);

        for (auto const &item: *properties) {

            auto key = item.first;
            auto property = item.second;

            // check if the key is already assigned and load that value
            // or create a new assignment
            if (preferences.isKey(key)) {
                auto loadedValue = preferences.getUChar(key);
                property->set(loadedValue);
            } else {
                preferences.putUChar(key, property->get());
            }

            // add a listener for updating the preferences value
            property->addListener([key](uint8_t, uint8_t _new) {
                preferences.putUChar(key, _new);
            });
        }
    }
}

#endif //ALARM_CLOCK_R2V6_STORAGE_H
