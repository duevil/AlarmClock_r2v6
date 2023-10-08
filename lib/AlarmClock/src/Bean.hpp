//
// Created by Malte on 05.10.2023.
//

#ifndef ALARM_CLOCK_BEAN_HPP
#define ALARM_CLOCK_BEAN_HPP


template<typename T>
class Bean {

    T value{};
    const char *name;
    Preferences &preferences;

protected:

    virtual ~Bean() = default;

    virtual T get(Preferences &, const char *) = 0;

    virtual void put(Preferences &, const char *, T) = 0;

public:

    Bean(const char *name, Preferences &preferences) : name(name), preferences(preferences) {}

    T get() const { return value; }

    void set(T val) { put(preferences, name, value = val); }

    void load() {
        if (preferences.isKey(name)) value = get(preferences, name);
        else put(preferences, name, value);
    }

    void reset() { preferences.remove(name); }

    explicit operator T() const { return get(); }

    virtual Bean<T> &operator=(T val) = 0;

};


class Uint8Bean : public Bean<uint8_t> {

    uint8_t get(Preferences &preferences, const char *name) override {
        return preferences.getUChar(name);
    }

    void put(Preferences &preferences, const char *name, uint8_t value) override {
        assert(preferences.putUChar(name, value));
    }

public:

    Uint8Bean(const char *name, Preferences &preferences) : Bean(name, preferences) {}

    Uint8Bean &operator=(uint8_t val) override {
        set(val);
        return *this;
    }

};

class BoolBean : public Bean<bool> {

    bool get(Preferences &preferences, const char *name) override {
        return preferences.getBool(name);
    }

    void put(Preferences &preferences, const char *name, bool value) override {
        assert(preferences.putBool(name, value));
    }

public:

    BoolBean(const char *name, Preferences &preferences) : Bean(name, preferences) {}

    BoolBean &operator=(bool val) override {
        set(val);
        return *this;
    }

};

class StringBean : public Bean<String> {

    String get(Preferences &preferences, const char *name) override {
        return preferences.getString(name);
    }

    void put(Preferences &preferences, const char *name, String value) override {
        assert(preferences.putString(name, value));
    }

public:

    StringBean(const char *name, Preferences &preferences) : Bean(name, preferences) {}

    StringBean &operator=(String val) override {
        set(val);
        return *this;
    }

};


#endif //ALARM_CLOCK_BEAN_HPP
