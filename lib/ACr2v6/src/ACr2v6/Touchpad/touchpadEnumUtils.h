//
// Created by Malte on 08.03.2023.
//

#ifndef ALARM_CLOCK_R2V6_PADS_H
#define ALARM_CLOCK_R2V6_PADS_H


namespace touchpad {

    using pad_vector_t = std::vector<std::unique_ptr<Touchpad>>;

    /**
     * @brief Lazily creates a vector with unique touchpad pointers from the pad enum
     * @return The created vector
     */
    inline const pad_vector_t &touchpads_from_enum() {
        static const pad_vector_t padVector{[]() {
            pad_vector_t vector{};
            std::vector<Touchpad> v{
            };
            for (pad_t pad: {
                    pad_t::MID,
                    pad_t::LEFT,
                    pad_t::RIGHT,
                    pad_t::UP,
                    pad_t::DOWN,
            }) {
                vector.emplace_back(new Touchpad{pad});
                // use new for creating a new unique pointer as make_unique is not available
            }
            return vector;
        }()};
        return padVector;
    }

    /**
     * @brief Converts a pad enum value to its string representation
     * @param pad The enum value to be converted to a string
     * @return A string representation of the enum value
     */
    constexpr const char *pad_to_string(pad_t pad) {
        return pad == pad_t::MID ? "MID" :
               pad == pad_t::LEFT ? "LEFT" :
               pad == pad_t::RIGHT ? "RIGHT" :
               pad == pad_t::UP ? "UP" :
               pad == pad_t::DOWN ? "DOWN" :
               "NONE";
    }

}


#endif //ALARM_CLOCK_R2V6_PADS_H
