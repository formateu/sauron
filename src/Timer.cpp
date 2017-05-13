//
// Created by Przemyslaw Kopanski on 13.05.17
//
#include "Timer.h"

Timer::~Timer() {
    stop();
}

void Timer::stop() {
    running = false;

    if (timerThread.joinable()) {
        timerThread.join();
    }
}
