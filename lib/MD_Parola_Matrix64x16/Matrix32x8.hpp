//
// Created by Malte on 16.09.2023.
//

#ifndef MD_PAROLA_MATRIX_32X8_HPP
#define MD_PAROLA_MATRIX_32X8_HPP


namespace md_parola_matrix32x8 {

    /**
     * @brief A class for controlling a 32x8 LED matrix display.
     * The class uses the MD_Parola library.
     */
    class Matrix32x8 {

        static constexpr uint8_t SCROLL_SPACING{32};
        enum class Animation {
            NONE,
            SCROLL_NEXT,
            SCROLL_NEXT_ONGOING,
            SCROLL_PREV,
            SCROLL_PREV_ONGOING,
            SCROLL_FINISH,
        };
        using TextSupplier = std::function<std::string()>;
        struct Tab {
            uint8_t index;
            TextSupplier textSupplier;
            Tab *prev;
            Tab *next;
        };
        bool setupDone{false};
        MD_Parola md;
        Animation animation{Animation::NONE};
        std::vector<Tab> tabs;
        Tab *currentTab{nullptr};
        Tab *scrollTo{nullptr};
        Animation lastAnimation{Animation::NONE};

    public:

        template<typename ...TextSuppliers>
        explicit Matrix32x8(uint8_t csPin, TextSuppliers...textSuppliers) : md(MD_MAX72XX::FC16_HW, csPin, 4) {
            for (const auto &ts: std::vector<TextSupplier>{textSuppliers...}) {
                tabs.push_back(Tab{(uint8_t) tabs.size(), ts, nullptr, nullptr});
            }
            for (int i = 0; i < tabs.size(); ++i) {
                tabs[i].prev = &tabs[(i - 1 + tabs.size()) % tabs.size()];
                tabs[i].next = &tabs[(i + 1) % tabs.size()];
            }
            currentTab = &tabs.front();
        }

        /**
         * @brief Sets up the matrix display.
         * Sets the font, text alignment, character spacing, scroll spacing and text effect.
         */
        bool setup() {
            assert(!setupDone);
            if (md.begin()) {
                md.setIntensity(0);
                md.setFont(matrix_font);
                md.setTextAlignment(PA_CENTER);
                md.setCharSpacing(0);
                md.setScrollSpacing(SCROLL_SPACING);
                md.setTextEffect(PA_NO_EFFECT, PA_NO_EFFECT);
                md.displayReset();
                md.displayClear();
                return setupDone = true;
            }
            return false;
        }

        /**
         * @brief Sets the text to be displayed and controls the scrolling animation.
         * This function is non-blocking and therefore should be called regularly.
         * @param text The text to be displayed
         */
        void loop() {
            assert(setupDone);
            const std::string &string = currentTab->textSupplier();
            md.setTextBuffer(string.c_str());
            if (md.displayAnimate()) {
                switch (animation) {
                    case Animation::NONE:
                        break;
                    case Animation::SCROLL_NEXT:
                        md.setTextEffect(PA_NO_EFFECT, PA_SCROLL_LEFT);
                        animation = Animation::SCROLL_NEXT_ONGOING;
                        break;
                    case Animation::SCROLL_NEXT_ONGOING:
                        md.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
                        currentTab = scrollTo;
                        animation = Animation::SCROLL_FINISH;
                        break;
                    case Animation::SCROLL_PREV:
                        md.setTextEffect(PA_NO_EFFECT, PA_SCROLL_RIGHT);
                        animation = Animation::SCROLL_PREV_ONGOING;
                        break;
                    case Animation::SCROLL_PREV_ONGOING:
                        md.setTextEffect(PA_SCROLL_RIGHT, PA_NO_EFFECT);
                        currentTab = scrollTo;
                        animation = Animation::SCROLL_FINISH;
                        break;
                    case Animation::SCROLL_FINISH:
                        md.setTextEffect(PA_NO_EFFECT, PA_NO_EFFECT);
                        animation = Animation::NONE;
                        break;
                }
                md.displayReset();
            }
        }

        /**
         * @brief Overrides the text to be displayed, ignoring the text supplier and the scrolling animation.
         * @param text The text to be displayed
         */
        void overrideText(const char *text) {
            assert(setupDone);
            md.setTextBuffer(text);
            md.displayReset();
            md.displayAnimate();
        }

        /**
         * @brief Shutdowns the display.
         * @param shutdown True to shutdown the display, false to turn it on
         */
        void shutdown(bool shutdown) {
            assert(setupDone);
            md.displayShutdown(shutdown);
        }

        /**
         * @brief Sets the brightness of the display.
         * @param brightness The brightness to be set (0-15)
         */
        void setBrightness(uint8_t brightness) {
            assert(setupDone);
            md.setIntensity(brightness);
        }

        /**
         * @brief Sets the brightness of the display to the maximum value (15).
         */
        void setMaxBrightness() {
            assert(setupDone);
            md.setIntensity(15);
        }

        /**
         * @brief Scrolls to the next tab.
         */
        void scrollNext() {
            animation = lastAnimation = Animation::SCROLL_NEXT;
            scrollTo = currentTab->next;
        }

        /**
         * @brief Scrolls to the previous tab.
         */
        void scrollPrev() {
            animation = lastAnimation = Animation::SCROLL_PREV;
            scrollTo = currentTab->prev;
        }

        /**
         * @brief Scrolls to the start of the tab list.
         */
        void scrollToStart() {
            auto pTab = &tabs.front();
            // scroll to start only if not already at start
            if (currentTab == pTab) return;
            // scroll direction against the last direction
            if (lastAnimation == Animation::SCROLL_NEXT) {
                animation = Animation::SCROLL_PREV;
                scrollTo = currentTab->prev;
            } else {
                animation = Animation::SCROLL_NEXT;
                scrollTo = currentTab->next;
            }
            scrollTo = pTab;
        }

        // delete copy constructor and assignment operator

        Matrix32x8(const Matrix32x8 &o) = delete;

        Matrix32x8 &operator=(const Matrix32x8 &o) = delete;

    };
}


#endif //MD_PAROLA_MATRIX_32X8_HPP
