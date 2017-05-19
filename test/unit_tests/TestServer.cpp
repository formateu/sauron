//
// Created by Mateusz Forc and Wiktor Franus on 19.05.17
//
#include <boost/test/unit_test.hpp>

#include "Server.h"
#include "ServerTestObj.h"
#include "Config.h"

BOOST_AUTO_TEST_CASE(create_server_with_nullptr_config) {
    // set up environment
    MessageBuffer msgBuffer;
    Connector *connector = new MockConnector(msgBuffer);
    ConfigBase *conf = nullptr;

    auto check_config_nullptr_exception = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), "Invalid pointer to config file");
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(ServerTestObj server(msgBuffer, "127.0.0.1", 7777, conf, connector),
                            std::runtime_error,
                            check_config_nullptr_exception);
}

BOOST_AUTO_TEST_CASE(server_handle_unexpected_message_main_loop) {
    // set up environment
    MessageBuffer msgBuffer;
    Connector *connector = new MockConnector(msgBuffer);
    ConfigBase *conf = new MockConfig();
    conf->m_ipVec.emplace_back("192.168.1.1");
    conf->m_ipVec.emplace_back("192.168.1.2");
    ServerTestObj server(msgBuffer, "127.0.0.1", 7777, conf, connector);

    // given
    // server with its first state
    // message with unexpected for server type
    Message msg;
    msg.m_type = MessageType::Finish;
    MessagePair mpair = {"192.168.1.1", msg};
    msgBuffer.push(mpair);

    // when
    // Unexpected message is received
    // main loop throws exception
    auto check_unexpected_msg_exception = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), "Server should not get that type of message from anyone.");
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(server.run(),
                            std::runtime_error,
                            check_unexpected_msg_exception);
}
