//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include <cerrno>

#include <boost/test/unit_test.hpp>

#include "Connector.h"
#include "MessageBuffer.h"

BOOST_AUTO_TEST_CASE(create_connector_with_restricted_port_number) {
    MessageBuffer msgBuffer;
    size_t port = 22;

    if(!getuid()) {
        // tests run as root
        BOOST_REQUIRE_NO_THROW(InternetConnector(msgBuffer, port));
    } else {
        auto check_binding_socket_failed_msg = [](const std::runtime_error &e) {
            BOOST_REQUIRE_EQUAL(e.what(), std::string("Binding socket failed"));
            return true;
        };

        BOOST_REQUIRE_EXCEPTION(InternetConnector(msgBuffer, port),
                                std::runtime_error,
                                check_binding_socket_failed_msg);

        BOOST_REQUIRE_EQUAL(errno, EACCES);
    }
}

BOOST_AUTO_TEST_CASE(create_connector_with_allowed_port_number) {
    MessageBuffer msgBuffer;
    size_t port = 5555;

    BOOST_REQUIRE_NO_THROW(InternetConnector(msgBuffer, port));
}

BOOST_AUTO_TEST_CASE(create_connector_port_already_in_use) {
    MessageBuffer msgBuffer;
    size_t port = 5555;

    InternetConnector connector1(msgBuffer, port);

    auto check_binding_socket_failed_msg = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), std::string("Binding socket failed"));
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(InternetConnector(msgBuffer, port),
                            std::runtime_error,
                            check_binding_socket_failed_msg);

    BOOST_REQUIRE_EQUAL(errno, EADDRINUSE);
}

BOOST_AUTO_TEST_CASE(send_with_invalid_ip_address) {
    // set up environment
    MessageBuffer msgBuffer;
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    Message msg(Init);

    //when
    std::string address = "invalid address";

    //then
    auto check_unrecognized_address_msg = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), std::string("Unrecognized IP address"));
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(connector.send(address, msg),
                            std::runtime_error,
                            check_unrecognized_address_msg);
}

BOOST_AUTO_TEST_CASE(send_with_valid_ip4_address) {
    // set up environment
    MessageBuffer msgBuffer;
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    Message msg(Init);

    //when
    std::string address = "127.0.0.1";

    //then
    BOOST_REQUIRE_NO_THROW(connector.send(address, msg));
}

BOOST_AUTO_TEST_CASE(send_with_valid_ip6_address) {
    // set up environment
    MessageBuffer msgBuffer;
    size_t port = 5555;
    InternetConnector connector(msgBuffer, port);

    // given
    Message msg(Init);

    //when
    std::string address = "::1";

    //then
    BOOST_REQUIRE_NO_THROW(connector.send(address, msg));
}