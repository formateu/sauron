//
// Created by Przemyslaw Kopanski
//
#ifndef SAURON_MESSAGEBUFFER_H
#define SAURON_MESSAGEBUFFER_H

#include <queue>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <condition_variable>

#include "Message.h"


class Semaphore {
public:
    void notify();

    void wait();

    bool tryWait();

private:
    boost::mutex mutex_;

    boost::condition_variable condition_;

    unsigned long count_ = 0;
};

class MessageBuffer {
public:
    MessageBuffer();

    ~MessageBuffer() {}

    void push(MsgSenderPair msg);

    MsgSenderPair pop();

    /**
     * Non-blocking pop, returns false if there's no element in queue
     * If there's element, then it will be provided to &result
     */
    bool tryPop(MsgSenderPair& result);

    using QueueType = std::priority_queue<MsgSenderPair, std::vector<MsgSenderPair>,
            std::function<bool(MsgSenderPair, MsgSenderPair)> >;

protected:
    QueueType m_queue;

    std::mutex m_mutex;

    Semaphore full;
};

#endif //SAURON_MESSAGEBUFFER_H
