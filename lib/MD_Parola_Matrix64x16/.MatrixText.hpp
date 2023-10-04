/*

//
// Created by Malte on 16.09.2023.
//

#ifndef MD_PAROLA_MATRIX_TEXT_HPP
#define MD_PAROLA_MATRIX_TEXT_HPP


namespace md_parola_matrix64x16 {
    class MatrixText {

    public:
        class Builder;

        class Literal;

        class Number;

        class Dot;

        class Colon;

        class DegreeSign;

        class Bell;

        inline static Builder builder();
        template<typename ...Texts>
        inline static MatrixText &joined(Texts &&...texts);
        virtual const char* get() const = 0;

        explicit operator const char*() const { return get(); }
    };

    class MatrixText::Literal : public MatrixText {
        const std::string &text;

    public:
        explicit Literal(const std::string &text) : text(text) {}

        inline const char* get() const override { return text.c_str(); }
    };

    class MatrixText::Number : public MatrixText {
        const long &number;
        int digits;
        bool subscript;

        const char* get() const override {
            std::string text = std::to_string(number);
            if (text.length() < digits) { text.insert(0, digits - text.length(), '0'); }
            else if (text.length() > digits) { text = text.substr(text.length() - digits); }
            if (subscript) {
                // the subscript unicode characters are 192 for '0' to 201 for '9'
                // thus the subscript offset is 144
                for (char &c: text) { c += (char) 144; }
            }
            return text.c_str();
        }

    public:
        Number(const long &number, int digits, bool subscript = false)
                : number(number), digits(digits), subscript(subscript) {}
    };

    class MatrixText::Bell : public MatrixText {
        const bool &active;

        inline const char* get() const override {
            // '$' is the active bell symbol
            // '%' is the inactive bell symbol
            return active ? "$" : "%";
        }

    public:
        explicit Bell(const bool &active) : active(active) {}
    };

    class MatrixText::Dot : public MatrixText {
        inline const char* get() const override { return "."; }
    };

    class MatrixText::Colon : public MatrixText {
        inline const char* get() const override { return ":"; }
    };

    class MatrixText::DegreeSign : public MatrixText {
        inline const char* get() const override { return "*"; }
    };

    class MatrixText::Builder : public MatrixText {
        std::vector<std::reference_wrapper<MatrixText>> texts;

    public:
        Builder() = default;

        inline Builder &operator+(MatrixText &text) {
            texts.emplace_back(text);
            return *this;
        }

        inline Builder &operator+(MatrixText &&text) {
            texts.emplace_back(text);
            return *this;
        }

        inline Builder &operator+(std::string &text) {
            MatrixText &&t = Literal(text);
            texts.emplace_back(t);
            return *this;
        }

        const char* get() const override {
            std::string text;
            for (const MatrixText &t: texts) { text += t.get(); }
            return text.c_str();
        }
    };

    MatrixText::Builder MatrixText::builder() { return {}; }

    template<typename... Texts>
    MatrixText &MatrixText::joined(Texts &&... texts) { return (builder() + ... + texts); }
}


#endif //MD_PAROLA_MATRIX_TEXT_HPP

*/