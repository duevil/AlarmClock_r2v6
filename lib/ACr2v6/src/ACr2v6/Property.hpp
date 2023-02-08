//
// Created by Malte on 03.02.2023.
//

#ifndef ALARM_CLOCK_R2V6_PROPERTY_H
#define ALARM_CLOCK_R2V6_PROPERTY_H

#include <string>
#include <vector>
#include <functional>


template<typename T>
using ChangeListener = std::function<void(T, T)>;

/**
 * @brief Simple class for storing an uint_8 value
 * with the functionality of adding change listeners
 */
template<typename T>
class Property {

public:

    /**
     * @brief Set the value of the property
     * and, if and only if the new value is unequal to the old value, calls all attached listeners
     * @param value The new value of the property
     */
    void set(T value) {
        if (val != value) {
            for (const auto &listener: listeners) {
                listener(val, value);
            }
            val = value;
        }
    }

    /**
     * @brief Attached a listener to the property, which will be called when the property changes
     * @param listener A function to execute when the value of the property changes
     * @warning Listeners, once attached, can not be removed
     */
    void addListener(ChangeListener<T> const &listener) {
        listeners.push_back(listener);
    }

    /**
     * Returns the current value of the Property
     *
     * @return The current value
     */
    inline T get() const { return val; }

private:

    T val{};
    std::vector<ChangeListener<T>> listeners{};

};


#endif //ALARM_CLOCK_R2V6_PROPERTY_H
