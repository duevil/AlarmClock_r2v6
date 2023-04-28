//
// Created by Malte on 08.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_INITIALISABLE_HPP
#define ALARM_CLOCK_R2V6_INITIALISABLE_HPP


/**
 * @brief A simple class for objects having an initialize-method and an initialized-state
 */
class Initialisable {

public:

    /**
     * @brief Initializes this object
     */
    virtual void init() = 0;

protected:

    ~Initialisable() = default;

    /**
     * @brief Asserts that this object has not been initialized and sets the initialize-state to true
     */
    void setInit() {
        assert(!initialized);
        initialized = true;
    }

    /**
     * @brief Assert that this object was initialized and returns the initialize-state
     * @return Whether this object has been initialized
     */
    bool isInit() const {
        assert(initialized);
        return initialized;
    }

private:

    bool initialized{false};

};


#endif //ALARM_CLOCK_R2V6_INITIALISABLE_HPP
