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

    BOOST_REQUIRE_NO_THROW(InternetConnector(msgBuffer, port));

    auto check_binding_socket_failed_msg = [](const std::runtime_error &e) {
        BOOST_REQUIRE_EQUAL(e.what(), std::string("Binding socket failed"));
        return true;
    };

    BOOST_REQUIRE_EXCEPTION(InternetConnector(msgBuffer, port),
                            std::runtime_error,
                            check_binding_socket_failed_msg);

    BOOST_REQUIRE_EQUAL(errno, EADDRINUSE);

}

