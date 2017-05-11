//
// Created by Przemyslaw Kopanski
//

#include "MessageBuffer.h"


void Semaphore::notify() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    ++count_;
    condition_.notify_one();
}

void Semaphore::wait() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    while (!count_) // Handle spurious wake-ups.
        condition_.wait(lock);

    --count_;
}


MessageBuffer::MessageBuffer() {
    auto cmp = [](const MsgSenderPair& left, const MsgSenderPair& right) {
        return left.second.m_type == MessageType::Error;
    };

    m_queue = QueueType(cmp);
}


void MessageBuffer::push(MsgSenderPair msg) {
    m_mutex.lock();
        m_queue.push(msg);
    m_mutex.unlock();

    full.notify(); // element ready to consume
}


MsgSenderPair MessageBuffer::pop() {
    MsgSenderPair result;

    full.wait();

    m_mutex.lock();
        result = m_queue.top();
        m_queue.pop();
    m_mutex.unlock();

    return result;
}
