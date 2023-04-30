//
// Created by Malte on 28.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_PROPERTY_HPP
#define ALARM_CLOCK_R2V6_PROPERTY_HPP


/**
     * A class storing a value and notifying listeners when it changes.
     * @tparam T the type of the value
     */
template<typename T>
class Property {

public:
    using ChangeListener = std::function<void(const Property<T> &, T, T)>;

    /**
     * Creates a new property with a name and an initial value.
     * @param name the name of the property
     * @param initialValue the initial value
     */
    Property(const char *name, T initialValue) : name(name), value(initialValue) {}

    /**
     * Creates a new property with a name and an initial value of 0.
     * @param name the name of the property
     */
    explicit Property(const char *name) : Property(name, {}) {}

    const char *getName() const { return name; }

    T get() const { return value; }

    /**
     * Sets the value and notifies listeners if it changed.
     * @param newValue the new value
     */
    void set(T newValue) {
        if (newValue != value) {
            T oldValue = value;
            value = newValue;
            for (auto &changeListener: changeListeners) {
                changeListener(*this, oldValue, newValue);
            }
        }
    }

    /**
     * Adds a listener that is called when the value of the property changes.
     * @param listener the listener to add
     */
    void addChangeListener(const ChangeListener &listener) { changeListeners.push_back(listener); }

    explicit operator T() const { return get(); }

    template<typename U>
    explicit operator U() const { return static_cast<U>(get()); }

    Property &operator=(T newValue) {
        set(newValue);
        return *this;
    }

    Property &operator+=(T newValue) { return operator=(value + newValue); }

    Property &operator-=(T newValue) { return operator=(value - newValue); }

    Property &operator*=(T newValue) { return operator=(value * newValue);}

    Property &operator/=(T newValue) { return operator=(value / newValue);}

    Property &operator++() { return operator+=(1); }

    Property &operator--() { return operator-=(1); }

    T operator+(T other) const { return value + other; }

    T operator-(T other) const { return value - other; }

    T operator*(T other) const { return value * other; }

    T operator/(T other) const { return value / other; }

    // disable copy and assignment

    Property(const Property &other) = delete;
    Property &operator=(const Property &other) = delete;

private:

    const char *name{};
    T value;
    std::vector<ChangeListener> changeListeners{};

};


#endif //ALARM_CLOCK_R2V6_PROPERTY_HPP
