//
// Created by Malte on 09.10.2023.
//

#ifndef USER_INTERFACE_DISPLAY_HPP
#define USER_INTERFACE_DISPLAY_HPP


namespace UserInterface {

    class UIDisplay {

        using Handle = std::function<void(UIDisplay &)>;

        SSD1306Wire oled;
        OLEDDisplayUi ui{&oled};
        uint8_t cursor{0};
        std::vector<Handle> handles;
        std::vector<FrameCallback> callbacks;

        friend class UIGraphics;

    public:

        OLEDDisplay &display = oled;

        /**
         * @brief A frame that can be displayed;
         *       it consists of a callback that is called when the frame is displayed
         *       and a handle that can be used process user input
         */
        struct Frame {
            FrameCallback callback;
            Handle handle;
        };

        UIDisplay(uint8_t address, uint8_t sda, uint8_t scl) : oled(address, sda, scl) {
            oled.setFont(Roboto_Mono_Light_10);
            ui.setTargetFPS(30);
            ui.disableAllIndicators();
            ui.disableAutoTransition();
            ui.setFrameAnimation(SLIDE_LEFT);
            ui.getUiState()->userData = &cursor;
        }

        /**
         * @brief Set the frames that should be displayed
         * @tparam Frames the frames that should be displayed
         * @param frames the frames that should be displayed
         */
        template<typename ...Frames>
        void setFrames(Frames ...frames) {
            handles = {frames.handle...};
            callbacks = {frames.callback...};
            ui.setFrames(callbacks.data(), sizeof...(Frames));
        }

        /**
         * @brief Initialize the display
         */
        void setup() {
            oled.init();
            oled.flipScreenVertically();
            oled.clear();
        }

        /**
         * @brief Run UI updates and call the current frame's handle
         * @return true if the current frame is not animating
         */
        bool loop() {
            if (ui.getUiState()->frameState == FIXED) {
                auto frame = ui.getUiState()->currentFrame;
                handles[frame](*this);
            }
            ui.update();
            return ui.getUiState()->frameState == FIXED;
        }

        /**
         * @brief Draw an animated progress bar with a message and the progress percentage
         * @param progress the current progress in percent 0-100
         * @param msg the message that should be displayed; if it is too long it will be wrapped
         */
        void drawBootAnimation(uint8_t progress, const String &msg) {
            static uint8_t lastProgress = 0;
            // don't instantly jump to the next step but animate the progress
            while (lastProgress <= progress) {
                oled.clear();
                oled.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
                oled.drawStringMaxWidth(64, 8, 120, msg);
                oled.drawString(64, 40, String(progress) + '%');
                oled.drawProgressBar(2, 50, 124, 8, lastProgress);
                oled.display();
                lastProgress++;
                // add a small delay to visualize the progress animation
                if (lastProgress >= 100) delay(500);
                else if (lastProgress == progress) delay(250);
                else delay(25);
            }
        }

        /**
         * @brief Transition to a new frame and reset the cursor to 0
         * @param frame the new frame
         */
        void transitionToFrame(uint8_t frame) {
            ui.transitionToFrame(frame);
            cursor = 0;
        }

        /**
         * @brief Set the current cursor value
         * @param value the new cursor value
         */
        void setCursor(uint8_t value) { cursor = value; }

        /**
         * @brief Get the current cursor value
         * @return the current cursor value
         */
        uint8_t getCursor() const { return cursor; }

        // delete copy constructor and assignment operator

        UIDisplay(const UIDisplay &) = delete;

        UIDisplay &operator=(const UIDisplay &) = delete;

    };

}


#endif //USER_INTERFACE_DISPLAY_HPP
