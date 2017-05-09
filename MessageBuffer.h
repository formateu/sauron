//
// Created by Wiktor Franus and Mateusz Forc on 04.05.17.
//

#ifndef SAURON_MESSAGEBUFFER_H
#define SAURON_MESSAGEBUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Message.h"


class Semaphore {
public:
    void notify();
    void wait();

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0;
};

class MessageBuffer {
public:
    MessageBuffer();
    ~MessageBuffer() {}

    void push(MsgSenderPair msg);
    MsgSenderPair pop();
    using QueueType = std::priority_queue<MsgSenderPair, std::vector<MsgSenderPair>,
                      std::function<bool(MsgSenderPair, MsgSenderPair)> >;

protected:
    QueueType m_queue;
    std::mutex m_mutex;
    Semaphore full;
};

#endif //SAURON_MESSAGEBUFFER_H
