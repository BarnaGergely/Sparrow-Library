#ifndef SparrowTimer_cpp
#define SparrowTimer_cpp

#include <SparrowTimer.h>

SparrowTimer::SparrowTimer() {}

void SparrowTimer::setInterval(unsigned long delayMillis, TimerCallback function) {
    this->delay = delayMillis;
    this->callback = function;
    status = true;  // enable timer after initialization
}

void SparrowTimer::changeInterval(unsigned long delayMillis) {
    this->delay = delayMillis;
}

unsigned long SparrowTimer::getInterval() {
    return delay;
}

void SparrowTimer::run() {
    unsigned long currentMillis = millis();
    if (status) {
        if (currentMillis - prevMillis >= delay) {
            prevMillis = currentMillis;
            (*callback)();
        }
    }
}

#endif