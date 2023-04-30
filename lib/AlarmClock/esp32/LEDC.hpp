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

public:
    enum class Resolution : uint8_t {
        BITS_1 = 1,
        BITS_2 = 2,
        BITS_4 = 4,
        BITS_8 = 8,
        BITS_10 = 10,
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
     * @brief Sets up the LEDC channel.
     */
    void setup() const {
        DEBUG_F("pin=%d channel=%d resolution=%d", pin, channel, resolution);
        auto freq = ledcSetup(channel, 5000, this->resolution);
        assert(freq != 0 && "Could not setup LEDC channel!");
        ledcAttachPin(pin, channel);
    }

    void setDuty(uint32_t duty) const { ledcWrite(channel, duty); }

    uint32_t getDuty() const { return ledcRead(channel); }

    void toggleOff() const { ledcWrite(channel, 0); }

    void toggleOn() const { ledcWrite(channel, (1 << resolution) - 1); }

    void operator++() const { setDuty((ledcRead(channel) + 1) % (1 << resolution)); }

    void operator--() const { setDuty((ledcRead(channel) - 1 + (1 << resolution)) % (1 << resolution)); }

    // disable copy constructor and assignment operator

    LEDC(const LEDC &other) = delete;
    LEDC &operator=(const LEDC &other) = delete;

private:
    static constexpr uint8_t CHANNEL_COUNT = 16;

    static std::array<bool, CHANNEL_COUNT> channelsInUse;
    const uint8_t pin;
    const uint8_t resolution;
    const uint8_t channel;

    /**
     * @brief Searches for a free LEDC channel and marks it as used.
     *
     * @return The number of the free LEDC channel.
     */
    static uint8_t getFreeChannel() {
        for (int i = 0; i < CHANNEL_COUNT; ++i) {
            if (!channelsInUse[i]) {
                channelsInUse[i] = true;
                return i;
            }
        }
        assert(false && "No free LEDC channel left!");
    }

};

std::array<bool, LEDC::CHANNEL_COUNT> LEDC::channelsInUse{};


#endif //ALARM_CLOCK_R2V6_LEDC_HPP
