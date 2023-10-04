//
// Created by Malte on 16.09.2023.
//

#ifndef MD_PAROLA_MATRIX_64X16_HPP
#define MD_PAROLA_MATRIX_64X16_HPP


namespace md_parola_matrix64x16 {
    class Matrix64x16 {

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

    public:

        template<typename ...TextSuppliers>
        explicit Matrix64x16(uint8_t csPin, TextSuppliers...textSuppliers) : md(MD_MAX72XX::FC16_HW, csPin, 4) {
            for (const auto &ts: std::vector<TextSupplier>{textSuppliers...}) {
                tabs.push_back(Tab{(uint8_t) tabs.size(), ts, nullptr, nullptr});
            }
            for (int i = 0; i < tabs.size(); ++i) {
                tabs[i].prev = &tabs[(i - 1 + tabs.size()) % tabs.size()];
                tabs[i].next = &tabs[(i + 1) % tabs.size()];
            }
            currentTab = &tabs.front();
        }

        void setup() {
            assert(!setupDone);
            md.begin();
            md.setIntensity(0);
            md.setFont(matrix_font);
            md.setTextAlignment(PA_CENTER);
            md.setCharSpacing(0);
            md.setScrollSpacing(SCROLL_SPACING);
            md.setTextEffect(PA_NO_EFFECT, PA_NO_EFFECT);
            md.displayReset();
            md.displayClear();
            setupDone = true;
        }

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
                        currentTab = currentTab->next;
                        animation = Animation::SCROLL_FINISH;
                        break;
                    case Animation::SCROLL_PREV:
                        md.setTextEffect(PA_NO_EFFECT, PA_SCROLL_RIGHT);
                        animation = Animation::SCROLL_PREV_ONGOING;
                        break;
                    case Animation::SCROLL_PREV_ONGOING:
                        md.setTextEffect(PA_SCROLL_RIGHT, PA_NO_EFFECT);
                        currentTab = currentTab->prev;
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

        void overrideText(const char *text) {
            assert(setupDone);
            md.setTextBuffer(text);
            md.displayReset();
            md.displayAnimate();
        }

        void shutdown(bool shutdown) {
            assert(setupDone);
            md.displayShutdown(shutdown);
        }

        void setBrightness(uint8_t brightness) {
            assert(setupDone);
            md.setIntensity(brightness);
        }

        void setMaxBrightness() {
            assert(setupDone);
            md.setIntensity(15);
        }

        void scrollNext() { animation = Animation::SCROLL_NEXT; }

        void scrollPrev() { animation = Animation::SCROLL_PREV; }

        // delete copy constructor and assignment operator

        Matrix64x16(const Matrix64x16 &o) = delete;
        Matrix64x16 &operator=(const Matrix64x16 &o) = delete;

    };
}


#endif //MD_PAROLA_MATRIX_64X16_HPP
