#ifndef SparrowTimer_h
#define SparrowTimer_h

/*
 * TODO: documentation
 * TODO: own millis
 * TODO: debug
 */

#include <Arduino.h>

/// timer callback function type
const typedef void (*TimerCallback)(void);

class SparrowTimer {
public:
    SparrowTimer();

    /**
     * Init timer to call function every delay milliseconds
     * @param delay delay in milliseconds between two callback calls
     * @param function Callback function, what cloud be called every delay milliseconds
     */
    void setInterval(unsigned long delay, TimerCallback function);

    /**
     * Update timer interval
     * @param delay new delay in milliseconds between two callback calls
     */
    void changeInterval(unsigned long delay);

    /**
     * It has to be called in the void loop for the timer to work
     */
    unsigned long getInterval();

    void run();

private:
    /// Timer status, it enables after initialization
    bool status = false;
    /// delay in milliseconds between two callback calls
    unsigned long delay;
    /// last callback call in milliseconds
    unsigned long prevMillis = 0;
    /// Callback function, what cloud be called delay frequency
    TimerCallback callback = nullptr;
};

#endif