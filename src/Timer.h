//
// Created by Przemyslaw Kopanski
//
#ifndef _SAURON_TIMER_H_
#define _SAURON_TIMER_H_
#include <chrono>
#include <thread>

/**
 * Timer object, running one function repeatedly
 */
class Timer {
public:
    /**
     * Runs action periodicly with given period (in seconds)
     */
    template<class Callable, class... Arguments>
    Timer(int period, Callable&& f, Arguments&&... args)
      : running(true) {
        std::function<typename std::result_of<callable(Arguments...)>::type()>
            task(std::bind(std::forward<Callable>(f), std::forward<Arguments>(args)...));

        timerThread = std::thread([&running, period, task]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::seconds(period));
                task();
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
