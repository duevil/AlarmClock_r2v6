//
// Created by Malte on 28.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_AVERAGED_VALUE_HPP
#define ALARM_CLOCK_R2V6_AVERAGED_VALUE_HPP


/**
 * A class storing a value averaged over the last n values.
 * @tparam T the type of the values
 * @tparam n the number of values to average
 */
template<typename T, size_t n>
class AveragedValue {

private:
    T value{};
    std::array<T, n> values{};
    size_t initialReads{n};

public:
    AveragedValue() = default;

    /**
     * @return the current average
     */
     T get() const { return value; }

    /**
     * Updates the average with a new value.
     * @param newValue the new value
     */
    typename std::enable_if<std::is_arithmetic<T>::value>::type
    update(T newValue) {
        if (initialReads > 0) {
            // fill all uninitialized values
            for (size_t i = n - initialReads; i < n; ++i) values[i] = newValue;
            initialReads--;
        } else {
            // shift all values one to the left
            std::copy(std::begin(values) + 1, std::end(values), std::begin(values));
            values[n - 1] = newValue;
        }
        // calculate the new average
        value = std::accumulate(std::begin(values), std::end(values), (T) 0.0) / n;
    }

    /**
     * Implicit conversion to the current average.
     * @return the current average
     */
     operator T() const { return get(); } // NOLINT(google-explicit-constructor)

    /**
     * Updates the average with a new value.
     * @param newValue the new value
     * @return the current average
     */
     AveragedValue &operator=(T newValue) {
        update(newValue);
        return *this;
    }
};


#endif //ALARM_CLOCK_R2V6_AVERAGED_VALUE_HPP
