#include <boost/test/unit_test.hpp>

#include "Message.h"
#include "MessageBuffer.h"
#include "Connector.h"
#include "ClientState.h"
#include "Client.h"

BOOST_AUTO_TEST_CASE(client_changes_state_to_desired_from_init) {
    // setup
    MessageBuffer msgBuffer;
    // client is responsible for freeing Connector
    Connector *connector = new MockConnector(msgBuffer);
    Client client(msgBuffer, "127.0.0.1", 7777, connector, ClientState::INIT_PHASE_FIRST);

    // prepare messeges in queue for client to receive
    // (this test is quite simple, there could be another thread
    // pushing messages into the queue with appropriate delays)
    Message initMessage;
    initMessage.m_type = MessageType::Init;

    msgBuffer.push({"192.168.1.2", initMessage});

    // run client
    std::thread clientThread([&client]() {
        client.run();
    });

    // wait to ensure messeges were processed
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // terminate the client's loop and the client itself
    client.stop();
    clientThread.join();

    // check if client changed state
    BOOST_TEST(client.getClientState() == ClientState::INIT_PHASE_SECOND);
}
