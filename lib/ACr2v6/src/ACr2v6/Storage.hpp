//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_STORAGE_H
#define ALARM_CLOCK_R2V6_STORAGE_H

#include <Preferences.h>
#include <utility>


namespace ac_r2v6 {

    /**
     * Manages the Preferences storage reading and updating
     */
    class Storage final : private Initialisable {

    public:

        /**
         * constructor; constructs a new storage object for the given Preferences namespace and properties
         */
        explicit Storage(const Property<uint8_t>::Map &properties, const char *preferencesName)
                : preferencesName(preferencesName), properties(properties) {}

        /**
         * @brief Initialises the internal Preferences Storage and syncs it with the properties;
         * if a property already exists in the Storage, it's value will be loaded, else if will be newly assigned
         */
        void init() override {
            DEBUG_SIMPLE("Preferences initialization start");
            setInit();

            DEBUG("Opening namespace: ", preferencesName);
            auto b = preferences.begin(preferencesName);
            assert(b && "Opening Preferences failed");

            for (auto const &item: properties) {

                auto key{item.first};
                auto &property{item.second};

                // check if the key is already assigned and load that value
                // or create a new assignment
                if (preferences.isKey(key)) {
                    auto value{preferences.getUChar(key)};
                    property.set(value);
                } else {
                    preferences.putUChar(key, property.get());
                }

                // add a listener for updating the preferences value
                property.addListener([key, this](uint8_t, uint8_t value) {
                    DEBUG("Updating preferences: ", key);
                    DEBUG("New value: ", value);
                    preferences.putUChar(key, value);
                });
            }

            DEBUG_SIMPLE("Preferences initialization end");
        }

        /**
         * @brief Reads a value from the storage
         * @tparam T The type of the value to read
         * @param key The key of the value inside the storage
         * @return The value or a default value if the key does not exists
         * @note Currently only reading uint8_t and String is supported
         */
        template<typename T>
        T read(const char *key) {
            if (isInit()) {
                if (std::is_same<T, uint8_t>()) return preferences.getUChar(key);
                if (std::is_same<T, String>()) return preferences.getString(key);
            }

            return static_cast<T>(assert(false));
        }

        /**
         * @brief Writes a value on the storage
         * @tparam T The type of the value to write
         * @param key The key for the value inside the storage
         * @param value The value to be written
         * @note Currently only writing uint8_t, const char * and String is supported
         */
        template<typename T>
        void write(const char *key, T value) {
            if (!isInit()) return;
            else if (std::is_same<T, uint8_t>()) preferences.putUChar(key, value);
            else if (std::is_same<T, const char *>() || std::is_same<T, String>()) preferences.putString(key, value);
        }

    private:

        const char *preferencesName;
        const Property<uint8_t>::Map &properties;
        Preferences preferences;

    };

}

#endif //ALARM_CLOCK_R2V6_STORAGE_H
