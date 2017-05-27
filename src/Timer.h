//
// Created by Przemyslaw Kopanski on 13.05.17
//
#ifndef _SAURON_TIMER_H_
#define _SAURON_TIMER_H_

#include <chrono>
#include <thread>
#include <iostream>

/**
 * Timer object, running one function repeatedly
 */
class Timer {
public:
    /**
     * Runs action periodicly with given period (in miliseconds)
     */
    template<class Callable, class... Arguments>
    Timer(unsigned period, Callable&& f, Arguments&&... args)
        : running(true)
    {
        auto task = [func = std::move(f), args...]() { func(std::forward(args)...); };

        timerThread = std::thread([this, period, task]() {

            while (running) {
                std::cout << "TIMER: sleeping time" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(period));

                if (running) { // prevents from executing after stop() was called
                    task();
                }
            }

        });
    }

    ~Timer();

    /**
     * Safe way to stop Timer task
     * Warning: stopping may last @period seconds
     */
    void stop();

private:
    std::thread timerThread;
    bool running;
};

#endif // _SAURON_TIMER_H_
