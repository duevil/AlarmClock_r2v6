//
// Created by Malte on 09.10.2023.
//

#ifndef USER_INTERFACE_GRAPHICS_HPP
#define USER_INTERFACE_GRAPHICS_HPP


namespace UserInterface {

    /**
     * @brief A BitMap in XBM format
     */
    struct BitMap {
        const uint8_t width;
        const uint8_t height;
        const uint8_t data[];
    };

    /**
     * @brief The lines of the display
     */
    enum class Line {
        L1,
        L2,
        L3,
        L4,
        L5
    };

    /**
     * @brief A wrapper for the FrameCallback parameter allowing for an easier definition of frames
     */
    struct UIGraphics {

        OLEDDisplay *const display;
        OLEDDisplayUiState *const state;
        const int16_t x;
        const int16_t y;

        /**
         * @brief Draw a line of text
         * @param line the line to draw the text on
         * @param text the text to draw
         * @param textAlignment the alignment of the text
         */
        void drawLine(
                Line line,
                const String &text,
                OLEDDISPLAY_TEXT_ALIGNMENT textAlignment = TEXT_ALIGN_LEFT
        ) const {
            display->setTextAlignment(textAlignment);
            uint8_t offset;
            switch (textAlignment) {
                case TEXT_ALIGN_LEFT:
                    offset = 0;
                    break;
                case TEXT_ALIGN_RIGHT:
                    offset = 128;
                    break;
                case TEXT_ALIGN_CENTER_BOTH:
                case TEXT_ALIGN_CENTER:
                    offset = 64;
                    break;
            }
            display->drawString((int16_t) (x + offset), (int16_t) (y + 12 * (uint8_t) line), text);
        }

        /**
         * @brief Draw a left aligned title on the first line
         * @param text the text to draw
         */
        void drawTitle(const String &text) const { drawLine(Line::L1, text); }

        /**
         * @brief Draw a left aligned title on the first line and any number of options on the following lines;
         *       the cursor position determines which option is highlighted
         * @tparam Options the options to draw
         * @param title the title to draw
         * @param options the options to draw
         */
        template<typename ...Options>
        void drawOptions(const String &title, Options ...options) const {
            drawTitle(title);
            display->setTextAlignment(TEXT_ALIGN_CENTER);
            std::array<String, sizeof...(Options)> _options{options...};
            // draw up to 4 options
            for (uint8_t i = 0; i < 4 && i < _options.size(); i++) {
                // calculate the index of the option to be drawn depending on the cursor position
                auto index = cursor() > 3 ? cursor() - 3 + i : i;
                if (index < _options.size()) {
                    auto option = _options[index];
                    if (option.length() > 0) {
                        auto text = index != cursor() ? option : "> " + option + " <";
                        display->drawString((int16_t) (64 + x), (int16_t) (y + 12 * (i + 1)), text);
                    }
                }
            }
        }

        /**
         * @brief Draw a value description pair with optional unit
         * @param line the line to draw the setter on
         * @param title the title of the setter
         * @param value the value of the setter
         * @param unit the unit of the setter
         * @param textAlignment the alignment of the text
         */
        void drawSetter(
                Line line,
                const String &title,
                uint8_t value,
                const String &unit = {},
                OLEDDISPLAY_TEXT_ALIGNMENT textAlignment = TEXT_ALIGN_LEFT
        ) const {
            display->setTextAlignment(textAlignment);
            auto text = title + ": ";
            if (value < 100) text += " ";
            if (value < 10) text += " ";
            text += value;
            if (unit) text += " " + unit;
            drawLine(line, text, textAlignment);
        }

        /**
         * @brief Draw a BitMap
         * @param xMove the x offset
         * @param yMove the y offset
         * @param bitmap the BitMap to draw
         */
        void drawBitMap(int16_t xMove, int16_t yMove, const BitMap &bitmap) const {
            display->drawXbm(
                    (int16_t) (x + xMove),
                    (int16_t) (y + yMove),
                    bitmap.width,
                    bitmap.height,
                    bitmap.data
            );
        }

        /**
         * @brief Get the current cursor value
         * @return the current cursor value
         */
        uint8_t cursor() const { return *(uint8_t *) state->userData; }
    };

}


#endif //USER_INTERFACE_GRAPHICS_HPP
