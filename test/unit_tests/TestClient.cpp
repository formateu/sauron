//
// Created by Przemyslaw Kopanski
//
#include <boost/test/unit_test.hpp>

#include "Message.h"
#include "MessageBuffer.h"
#include "Connector.h"
#include "Client.h"
#include "ClientTestObj.h"

BOOST_AUTO_TEST_CASE(client_handle_state_init_phase_first) {
    // set up environment
    MessageBuffer msgBuffer;
    Connector *connector = new MockConnector(msgBuffer);
    ClientTestObj client(msgBuffer, "127.0.0.1", 7777, connector);

    // given
    // client with its first state
    // message with init type
    client.setState(ClientState::INIT_PHASE_FIRST);
    Message msg;
    msg.m_type = MessageType::Init;
    MessagePair mpair = {"192.168.1.1", msg};

    // when
    // message is handled by handleStateInitPhaseFirst method
    client.runHandleStateInitPhaseFirst(mpair);

    // then
    // client must change its own state
    BOOST_TEST(client.getState() == ClientState::INIT_PHASE_SECOND);
}

BOOST_AUTO_TEST_CASE(client_handle_state_init_phase_first_unexpected_message) {
    // set up environment
    MessageBuffer msgBuffer;
    Connector *connector = new MockConnector(msgBuffer);
    ClientTestObj client(msgBuffer, "127.0.0.1", 7777, connector);

    // given
    // client with its first state
    // message with unexpected for client type
    client.setState(ClientState::INIT_PHASE_FIRST);
    Message msg;
    msg.m_type = MessageType::Measurement;
    MessagePair mpair = {"192.168.1.1", msg};

    // when
    // message is handled by handleStateInitPhaseFirst method
    client.runHandleStateInitPhaseFirst(mpair);

    // then
    // client must not change its state
    BOOST_TEST(client.getState() == ClientState::INIT_PHASE_FIRST);
}
