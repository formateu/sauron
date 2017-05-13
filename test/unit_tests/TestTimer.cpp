//
// Created by Przemyslaw Kopanski on 15.05.17
//
#include <thread>
#include <chrono>

#include <boost/test/unit_test.hpp>

#include "Timer.h"

BOOST_AUTO_TEST_CASE(timer_executes_few_times) {
    // given timer with task executing every 300ms
    int executionNum = 0;
    auto task = [&executionNum]() {
        ++executionNum;
    };

    Timer timer(300, task);

    // when timer is running for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.stop();

    // then it should execute 3 times
    BOOST_TEST(executionNum == 3);
}
