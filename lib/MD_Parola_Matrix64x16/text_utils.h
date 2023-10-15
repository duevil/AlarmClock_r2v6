//
// Created by Malte on 17.09.2023.
//

#ifndef MD_PAROLA_MATRIX_TEXT_UTILS_H
#define MD_PAROLA_MATRIX_TEXT_UTILS_H


/**
 * @brief Converts a number to a string with a given number of digits, appending leading zeros if necessary
 * and optionally converts the digits to subscript
 * @tparam T The type of the number
 * @param number The number to convert
 * @param digits The number of digits the resulting string should have
 * @param subscript Whether the digits should be converted to subscript
 * @return The resulting string
 */
template<typename T>
std::string numToStr(T number, uint8_t digits = 2, bool subscript = false) {
    auto s = std::to_string(number);
    if (s.length() < digits) s.insert(0, digits - s.length(), '0');
    else if (s.length() > digits) s = s.substr(0, digits);
    if (!isDigit(s.back())) s = '0' + s.substr(0, s.length() - 1);
    if (subscript) for (char &c: s) if (isDigit(c)) c = (char) (192 + c - '0');
    return s;
}

constexpr auto degreeSign = '*';
constexpr auto bellFilled = '$';
constexpr auto bellOutline = '%';

/**
 * @brief Return the bell character
 * @param filled Whether the bell should be filled or not
 * @return The bell character
 */
constexpr char c_bell(bool filled = true) { return filled ? bellFilled : bellOutline; }

/**
 * @brief A struct representing the bell character, storing whether it should be filled or not
 */
struct Bell {
    bool filled;

    explicit Bell(bool filled = true) : filled(filled) {}

    explicit constexpr operator char() const { return c_bell(filled); }
};


#endif //MD_PAROLA_MATRIX_TEXT_UTILS_H
