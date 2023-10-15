//
// Created by Malte on 28.04.2023.
//

#ifndef ALARM_CLOCK_R2V6_TIMER_HPP
#define ALARM_CLOCK_R2V6_TIMER_HPP


/**
 * @brief Class for creating and managing timers.
 *
 * Wraps the FreeRTOS xTimer-functions, providing a more convenient interface and allowing the use of lambdas.
 */
class ESP32_Timer {

public:
    using TimerCallback = std::function<void()>;

    /**
     * @brief Creates a new timer.
     *
     * Creates a new timer using FreeRTOS xTimerCreate and stores the callback in the static map.
     * @param name the name of the timer; used for debugging
     * @param milliseconds the period of the timer in milliseconds
     * @param reload whether the timer should reload after it has expired
     * @param callback the callback to call when the timer expires
     */
    ESP32_Timer(
            const char *name,
            uint32_t milliseconds,
            bool reload,
            const TimerCallback &callback
    ) : handle{xTimerCreate(
            name,
            pdMS_TO_TICKS(milliseconds),
            reload,
            nullptr,
            [](TimerHandle_t t) { callbacks[t](); }
    )} {
        assert(handle != nullptr && "Could not create timer: " && name);
        callbacks[handle] = callback;
    }

    /**
     * @brief Starts the timer using FreeRTOS xTimerStart.
     */
    void start() const {
        auto result = xTimerStart(handle, 0);
        assert(result == pdPASS && "Could not start timer " && pcTimerGetTimerName(handle));
    }

    /**
     * @brief Stops the timer using FreeRTOS xTimerStop.
     */
    void stop() const {
        auto result = xTimerStop(handle, 0);
        assert(result == pdPASS && "Could not stop timer" && pcTimerGetTimerName(handle));
    }

    /**
     * @brief Resets the timer using FreeRTOS xTimerReset.
     */
    void reset() const {
        auto result = xTimerReset(handle, 0);
        assert(result == pdPASS && "Could not reset timer" && pcTimerGetTimerName(handle));
    }

    /**
     * @brief Changes the period of the timer using FreeRTOS xTimerChangePeriod.
     * @param milliseconds the new period of the timer in milliseconds
     */
    void changePeriod(uint32_t milliseconds) const {
        auto result = xTimerChangePeriod(handle, pdMS_TO_TICKS(milliseconds), 0);
        assert(result == pdPASS && "Could not change timer period" && pcTimerGetTimerName(handle));
    }

    // disable copy constructor and assignment operator

    ESP32_Timer(const ESP32_Timer &o) = delete;

    ESP32_Timer &operator=(const ESP32_Timer &o) = delete;

private:
    /**
     * Because xTimer takes a function pointer as callback, we need to store the callbacks statically.
     * However, xTimerCreate returns a TimerHandle_t, which can be used to identify the timer
     * and therefore map it to the callback.
     */
    static std::unordered_map<TimerHandle_t, TimerCallback> callbacks;
    const TimerHandle_t handle;

};

std::unordered_map<TimerHandle_t, ESP32_Timer::TimerCallback> ESP32_Timer::callbacks{};


#endif //ALARM_CLOCK_R2V6_TIMER_HPP
