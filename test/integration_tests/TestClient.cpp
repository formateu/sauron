#include <boost/test/unit_test.hpp>

#include "Message.h"
#include "MessageBuffer.h"
#include "Connector.h"
#include "ClientState.h"
#include "Client.h"

BOOST_AUTO_TEST_CASE(client_changes_state_to_desired_from_init) {
    // setup
    std::shared_ptr<MessageBuffer> unusedMsgBuffer = std::make_shared<MessageBuffer>();
    std::shared_ptr<SplitMessageBuffer> msgBuffer = std::make_shared<SplitMessageBuffer>();
    // client is responsible for freeing Connector
    Connector *connector = new MockConnector(unusedMsgBuffer);
    Client client(msgBuffer, 7777, connector, ClientState::INIT_PHASE_FIRST);

    // prepare messeges in queue for client to receive
    // (this test is quite simple, there could be another thread
    // pushing messages into the queue with appropriate delays)
    Message initMessage;
    initMessage.m_type = MessageType::Init;

    msgBuffer->push({"192.168.1.2", initMessage});

    // run client
    std::thread clientThread([&client]() {
        client.run();
    });

    // wait to ensure messeges were processed
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    msgBuffer->push({"192.168.1.2", Message(MessageType::Ack)});

    // terminate the client's loop and the client itself
    client.stop();
    clientThread.join();

    // check if client changed state
    BOOST_TEST(client.getClientState() == ClientState::INIT_PHASE_SECOND);
}

BOOST_AUTO_TEST_CASE(client_ack_timeout_throws_exception) {
    // setup
    std::shared_ptr<MessageBuffer> unusedMsgBuffer = std::make_shared<MessageBuffer>();
    std::shared_ptr<SplitMessageBuffer> msgBuffer = std::make_shared<SplitMessageBuffer>();
    // client is responsible for freeing Connector
    Connector *connector = new MockConnector(unusedMsgBuffer);
    Client client(msgBuffer, 7777, connector, ClientState::INIT_PHASE_FIRST);

    Message initMessage;
    initMessage.m_type = MessageType::Init;
    msgBuffer->push({"192.168.1.2", initMessage});

    // run client
    std::thread clientThread([&client]() {
        client.run();
    });

    // awaiting to ensure ack timeout
    // as clients send InitOk and awaits for ack
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // join should not lock as client stops
    clientThread.join();
    BOOST_TEST(client.getClientState() == ClientState::INIT_PHASE_SECOND);
}

BOOST_AUTO_TEST_CASE(client_sends_measurements) {
    // setup
    // msgBuffer is buffer which client reads from
    // sendBuffer is buffer which client sends to
    std::shared_ptr<MessageBuffer> sendBuffer = std::make_shared<MessageBuffer>();
    std::shared_ptr<SplitMessageBuffer> msgBuffer = std::make_shared<SplitMessageBuffer>();
    Connector *connector = new MockConnector(sendBuffer);
    Client client(msgBuffer, 7777, connector, ClientState::INIT_PHASE_FIRST);

    Message clientMessage;
    Message msg;
    int i;
    std::array<unsigned char, 16> ipAddress =
        {255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3};
    // run client
    std::thread clientThread([&client]() {
        client.run();
    });

    // provide messages in order to move client to measurement state
    msg.m_type = MessageType::Init;
    i = 0;
    for (const auto& e : ipAddress) {
        msg.m_pipeAddress[i] = e;
        ++i;
    }
    msgBuffer->push({"192.168.1.3", msg});
    clientMessage = sendBuffer->pop().second;
    BOOST_TEST(clientMessage.m_type == MessageType::Ack);

    clientMessage = sendBuffer->pop().second;
    BOOST_TEST(clientMessage.m_type == MessageType::InitOk);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    msgBuffer->push({"192.168.1.3", Message(MessageType::Ack)});

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    msg.m_type = MessageType::Run;
    msg.m_activePeriod = 1;
    msg.m_inactivePeriod = 2;
    msgBuffer->push({"192.168.1.3", msg});
    clientMessage = sendBuffer->pop().second;
    BOOST_TEST(clientMessage.m_type == MessageType::Ack);

    // wait for 1st measurement
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    clientMessage = sendBuffer->pop().second;
    BOOST_TEST(clientMessage.m_type == MessageType::Measurement);
    for (i = 0; i < 16; ++i) {
        BOOST_TEST(clientMessage.m_pipeAddress[i] == ipAddress[i]);
    }
    msgBuffer->push({"192.168.1.3", Message(MessageType::Ack)});

    // terminate client
    client.stop();
    clientThread.join();
}
