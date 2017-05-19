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

BOOST_AUTO_TEST_CASE(msg_buffer_try_pop_success) {
    // given queue with 1 message
    MessageBuffer msgBuffer;
    msgBuffer.push({"127.0.0.1", Message(MessageType::Ack)});

    // when trying to pop an element
    MsgSenderPair msg;
    bool result = msgBuffer.tryPop(msg);

    // then message should be popped instantly
    BOOST_TEST(result == true);
    BOOST_TEST(msg.second.m_type == MessageType::Ack);
}

BOOST_AUTO_TEST_CASE(msg_buffer_empty_try_pop) {
    // given queue without any message
    MessageBuffer msgBuffer;

    // when trying to pop an element
    MsgSenderPair msg;
    bool result = msgBuffer.tryPop(msg);

    // then buffer shouldn't lock and return pop failure
    BOOST_TEST(result == false);
}
