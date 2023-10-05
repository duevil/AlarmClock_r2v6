//
// Created by Malte on 29.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_LEDC_HPP
#define ALARM_CLOCK_R2V6_LEDC_HPP


/**
 * @brief Class to control a LEDC channel.
 *
 * Wraps the ESP32 LEDC functions, using a fixed frequency of 5000 Hz and choosing a channel automatically
 * as well as providing easy access to the duty control.
 */
class LEDC {
    static constexpr auto CHANNEL_COUNT = 16;

    static uint16_t channelsInUse;
    const uint8_t pin;
    const uint8_t resolution;
    const uint8_t channel;
    const uint32_t maxDuty{(uint32_t) (1 << resolution) - 1};
    uint32_t curDuty{0};

    /**
     * @brief Searches for a free LEDC channel and marks it as used.
     *
     * @return The number of the free LEDC channel.
     */
    static uint8_t getFreeChannel() {
        auto channel = -1;
        for (auto i = 0; channel == -1 && i < CHANNEL_COUNT; ++i) {
            if ((channelsInUse & (1 << i)) == 0) {
                channelsInUse |= (1 << i);
                channel = i;
            }
        }
        assert(channel != -1 && "No free LEDC channel left!");
        return (uint8_t) channel; // to silence compiler warning
    }

public:
    enum class Resolution {
        BITS_1 = 1,
        BITS_2 = 2,
        BITS_3 = 3,
        BITS_4 = 4,
        BITS_5 = 5,
        BITS_6 = 6,
        BITS_7 = 7,
        BITS_8 = 8,
        BITS_9 = 9,
        BITS_10 = 10,
        BITS_11 = 11,
        BITS_12 = 12,
        BITS_13 = 13,
        BITS_14 = 14,
        BITS_15 = 15,
        BITS_16 = 16,
    };

    /**
     * @brief Constructor. Stores the pin and the resolution and chooses a free channel.
     * @param pin The pin to use.
     * @param resolution The resolution of the LEDC channel.
     */
    LEDC(uint8_t pin, Resolution resolution)
            : pin{pin},
              resolution{static_cast<uint8_t>(resolution)},
              channel{getFreeChannel()} {}

    /**
     * @brief Sets up the LEDC channel and attaches the pin.
     */
    void setup() const {
        auto freq = ledcSetup(channel, 5000, this->resolution);
        assert(freq != 0 && "Could not setup LEDC channel!");
        ledcAttachPin(pin, channel);
    }

    /**
     * @brief Detaches the pin and marks the channel as free.
     */
    void remove() const {
        ledcDetachPin(pin);
        channelsInUse &= ~(1 << channel);
    }

    void setDuty(uint32_t duty) { ledcWrite(channel, curDuty = duty % (maxDuty + 1)); }

    uint32_t getDuty() const { return curDuty; }

    uint32_t getMaxDuty() const { return maxDuty; }

    void toggleOff() { setDuty(0); }

    void toggleOn() { setDuty(maxDuty); }

    void operator++() { setDuty(getDuty() + 1); }

    void operator--() { setDuty(getDuty() + (maxDuty + 1) - 1); }

    // disable copy constructor and assignment operator

    LEDC(const LEDC &o) = delete;

    LEDC &operator=(const LEDC &o) = delete;

    /**
     * @brief Checks if any LEDC channel is available.
     * @return True if any LEDC channel is available, false otherwise.
     */
    static bool isAnyChannelAvailable() { return channelsInUse != (1 << CHANNEL_COUNT) - 1; }

};

uint16_t LEDC::channelsInUse{};


#endif //ALARM_CLOCK_R2V6_LEDC_HPP
