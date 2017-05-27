//
// Created by Przemyslaw Kopanski
//
#ifndef SAURON_MESSAGEBUFFER_H
#define SAURON_MESSAGEBUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Message.h"


class MessageBufferBase {
public:
    virtual void push(const MsgSenderPair&) = 0;
};

class Semaphore {
public:
    void notify();

    void wait();

    bool tryWait();

private:
    std::mutex mutex_;

    std::condition_variable condition_;

    unsigned long count_ = 0;
};

class MessageBuffer : public MessageBufferBase {
public:
    MessageBuffer();

    ~MessageBuffer() {}

    void push(const MsgSenderPair& msg);

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

class SplitMessageBuffer : public MessageBufferBase {
public:
    SplitMessageBuffer();

    ~SplitMessageBuffer() {}

    void push(const MsgSenderPair& msg);

    MsgSenderPair popAck();

    MsgSenderPair popNonAck();

protected:
    std::queue<MsgSenderPair> m_ack_queue;

    std::queue<MsgSenderPair> m_non_ack_queue;

    std::mutex m_mutex;

    Semaphore full_ack;

    Semaphore full_non_ack;
};

#endif //SAURON_MESSAGEBUFFER_H
