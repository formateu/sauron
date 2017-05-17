#include <thread>
#include <chrono>

#include <boost/test/unit_test.hpp>

#include "Message.h"
#include "MessageBuffer.h"

BOOST_AUTO_TEST_CASE(msg_buffer_nospinlock) {
    MessageBuffer msgBuffer;

    // trying to read while buffer is empty
    std::thread readerThread([&msgBuffer]() {
        msgBuffer.pop();
    });

    std::thread writerThread([&msgBuffer]() {
        // waiting a moment to ensure reader thread executes .pop()
        std::this_thread::sleep_for(std::chrono::seconds(1));
        msgBuffer.push({"127.0.0.1", Message()});
    });

    readerThread.join();
    writerThread.join();

    // spinlock haven't occured
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(msg_buffer_priority_works) {
    // given queue with 2 messages and 1 is Ack
    MessageBuffer msgBuffer;
    msgBuffer.push({"127.0.0.1", Message(MessageType::Init)});
    msgBuffer.push({"127.0.0.1", Message(MessageType::Ack)});

    // when popping an element
    Message msg = msgBuffer.pop().second;

    // then ack should be popped
    BOOST_TEST(msg.m_type == MessageType::Ack);
}
