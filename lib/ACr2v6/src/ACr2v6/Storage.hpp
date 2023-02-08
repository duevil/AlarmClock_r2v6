//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_STORAGE_H
#define ALARM_CLOCK_R2V6_STORAGE_H

#include <Preferences.h>
#include <map>


/**
 * @brief A simple struct for managing the Preferences storage reading and updating
 */
struct Storage {

    /**
     * @brief Initialises the internal Preferences Storage with the given name and syncs it with the given properties;
     * if a property already exists in the Storage, it's value will be loaded, else if will be newly assigned
     * @param properties The properties which will be synced
     */
    static void init(std::map<const char *const, Property<uint8_t> const &> const &properties) {
        static Preferences preferences;

        assert(preferences.begin(acc::PREFERENCES_NAME));

        for (auto const &item: properties) {

            auto key = item.first;
            auto property = item.second;

            // check if the key is already assigned and load that value
            // or create a new assignment
            if (preferences.isKey(key)) {
                auto value = preferences.getUChar(key);
                property.set(value);
            } else {
                preferences.putUChar(key, property.get());
            }

            // add a listener for updating the preferences value
            property.addListener([key](uint8_t, uint8_t value) {
                preferences.putUChar(key, value);
            });
        }
    }
};

#endif //ALARM_CLOCK_R2V6_STORAGE_H
