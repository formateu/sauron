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

bool Semaphore::tryWait() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    if (count_) {
      --count_;
      return true;
    }
    return false;
}

MessageBuffer::MessageBuffer() {
    auto cmp = [](const MsgSenderPair &left, const MsgSenderPair &right) {
        return right.second.m_type == MessageType::Ack;
    };

    m_queue = QueueType(cmp);
}

void MessageBuffer::push(const MsgSenderPair& msg) {
    m_mutex.lock();
        m_queue.push(msg);
    m_mutex.unlock();

    full.notify(); // element ready to consume
}

bool MessageBuffer::tryPop(MsgSenderPair& result) {
    if (!full.tryWait())
      return false;

    m_mutex.lock();
        result = m_queue.top();
        m_queue.pop();
    m_mutex.unlock();

    return true;
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

SplitMessageBuffer::SplitMessageBuffer() {}

void SplitMessageBuffer::push(const MsgSenderPair& msg) {
    m_mutex.lock();
        if (msg.second.m_type == MessageType::Ack)
            m_ack_queue.push(msg);
        else
            m_non_ack_queue.push(msg);
    m_mutex.unlock();

    if (msg.second.m_type == MessageType::Ack)
        full_ack.notify();
    else
        full_non_ack.notify();
}

MsgSenderPair SplitMessageBuffer::popAck() {
    MsgSenderPair result;

    full_ack.wait();

    std::unique_lock<std::mutex> lock(m_mutex);
        result = m_ack_queue.front();
        m_ack_queue.pop();

    return result;
}


MsgSenderPair SplitMessageBuffer::popNonAck() {
    MsgSenderPair result;

    full_non_ack.wait();

    m_mutex.lock();
        result = m_non_ack_queue.front();
        m_non_ack_queue.pop();
    m_mutex.unlock();

    return result;
}
