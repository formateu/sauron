//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include <cerrno>

#include <boost/test/unit_test.hpp>

#include "Connector.h"
#include "MessageBuffer.h"

BOOST_AUTO_TEST_CASE(create_connector_with_restricted_port_number) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();

    // given
    size_t port = 22;

    // when
    // access to privileged port is denied
    // then
    // Connector constructor should throw exception with proper message
    // otherwise
    // Connector construction succeeds
    if(!getuid()) {
        // tests run as root
        BOOST_REQUIRE_NO_THROW(InternetConnector(msgBuffer, port));
    } else {
        auto check_binding_socket_failed_msg = [](const std::runtime_error &e) {
            BOOST_REQUIRE_EQUAL(e.what(), "Binding socket failed");
            return true;
        };

        BOOST_REQUIRE_EXCEPTION(InternetConnector(msgBuffer, port),
                                std::runtime_error,
                                check_binding_socket_failed_msg);

        BOOST_REQUIRE_EQUAL(errno, EACCES);
    }
}

BOOST_AUTO_TEST_CASE(create_connector_with_allowed_port_number) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();

    // given
    size_t port = 5555;

    // when
    // Connector with unprivileged port is created
    // then
    // Connector construction succeeds
    BOOST_REQUIRE_NO_THROW(InternetConnector(msgBuffer, port));
}

BOOST_AUTO_TEST_CASE(create_connector_port_already_in_use) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();

    // given
    // Connector on port 5555
    size_t port = 5555;
    InternetConnector connector1(msgBuffer, port);

    // when
    // another Connector with the same port is created
    // then
    // Connector constructor should throw exception with proper message
    auto check_binding_socket_failed_msg = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), "Binding socket failed");
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(InternetConnector(msgBuffer, port),
                            std::runtime_error,
                            check_binding_socket_failed_msg);

    // and ERRNO is set to EADDRINUSE (ensure bind failed because of port already in use)
    BOOST_REQUIRE_EQUAL(errno, EADDRINUSE);
}

BOOST_AUTO_TEST_CASE(send_with_invalid_ip_address) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    // message with init type
    Message msg(Init);

    // when
    // sending message to unexisting/wrong address
    std::string address = "invalid address";

    // then
    // Connector should throw exception with proper message
    auto check_unrecognized_address_msg = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), "Unrecognized IP address");
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(connector.send(address, msg),
                            std::runtime_error,
                            check_unrecognized_address_msg);
}

BOOST_AUTO_TEST_CASE(send_with_valid_ip4_address) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    // message with init type
    Message msg(Init);

    // when
    // sending message to valid ipv4 address
    std::string address = "127.0.0.1";

    // then
    // send successes
    BOOST_REQUIRE_NO_THROW(connector.send(address, msg));
}

BOOST_AUTO_TEST_CASE(send_with_valid_ip6_address) {
    // set up environment
    std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    // message with init type
    Message msg(Init);

    // when
    // sending message to valid ipv6 address
    std::string address = "::1";

    // then
    // send successes
    BOOST_REQUIRE_NO_THROW(connector.send(address, msg));
}
