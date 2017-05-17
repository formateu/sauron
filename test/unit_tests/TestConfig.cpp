//
// Created by Grzegorz Staniszewski on 17.05.17
//
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

#include "Config.h"

const std::string fileName = "temp";

void before(const std::string &fileName, const std::string &config) {
    std::ofstream file;
    file.open (fileName);
    file << config;
    file.close();
}

void after(const std::string &fileName) {
    std::remove(fileName.c_str());
}

BOOST_AUTO_TEST_CASE(read_config_no_errors) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
            "client-work: 20\n"
            "ip:\n"
            "  - 192.168.1.1\n"
            "  - 192.168.1.2";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_NO_THROW(config.read());
    BOOST_TEST(config.clientSleepSeconds == 10);
    BOOST_TEST(config.clientWorkSeconds == 20);
    BOOST_TEST(config.addresses.size() == 2);
    BOOST_CHECK(config.addresses.find("192.168.1.1") != config.addresses.end());
    BOOST_CHECK(config.addresses.find("192.168.1.2") != config.addresses.end());

    after(fileName);
}

BOOST_AUTO_TEST_CASE(read_config_invalid_param_client_sleep) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: ERROR\n"
            "client-work: 20\n"
            "ip:\n"
            "  - 192.168.1.1\n"
            "  - 192.168.1.2";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_THROW(config.read(), std::invalid_argument);

    after(fileName);
}

BOOST_AUTO_TEST_CASE(read_config_invalid_param_client_work) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
            "client-work: ERROR\n"
            "ip:\n"
            "  - 192.168.1.1\n"
            "  - 192.168.1.2";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_THROW(config.read(), std::invalid_argument);

    after(fileName);
}



BOOST_AUTO_TEST_CASE(read_config_ip_contain_duplicates) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
            "client-work: 20\n"
            "ip:\n"
            "  - 192.168.1.1\n"
            "  - 192.168.1.2\n"
            "  - 192.168.1.2";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_THROW(config.read(), std::invalid_argument);

    after(fileName);
}

