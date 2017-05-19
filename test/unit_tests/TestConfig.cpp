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

BOOST_AUTO_TEST_CASE(read_config_ipV4_no_errors) {
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
    BOOST_TEST(config.m_ipVec.size() == 2);
    BOOST_CHECK(config.m_ipVec[0] == "192.168.1.1");
    BOOST_CHECK(config.m_ipVec[1] == "192.168.1.2");

    after(fileName);
}

BOOST_AUTO_TEST_CASE(read_config_ipV4_invalid_param_client_sleep) {
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

BOOST_AUTO_TEST_CASE(read_config_ipV4_invalid_param_client_work) {
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

BOOST_AUTO_TEST_CASE(read_config_ipV4_ip_contain_duplicates) {
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

BOOST_AUTO_TEST_CASE(test_ipv6_check_format) {
    // when
    MockConfig config;

    //then
    BOOST_CHECK_EQUAL(true, config.is_ipv6_correct_format("1111:2222:3333:4444:5555:6666:7777:8888"));
    BOOST_CHECK_EQUAL(true, config.is_ipv6_correct_format("aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh"));
    BOOST_CHECK_EQUAL(true, config.is_ipv6_correct_format("0000:0000:0000:0000:0000:ffff:c0a8:0101"));

    BOOST_CHECK_EQUAL(false, config.is_ipv6_correct_format("::ffff:c0a8:101"));
    BOOST_CHECK_EQUAL(false, config.is_ipv6_correct_format("123:0000:0000:0000:0000:0000:0000:0000"));
    BOOST_CHECK_EQUAL(false, config.is_ipv6_correct_format("0000:0000:0000:0000:0000:0000:0000:123"));
    BOOST_CHECK_EQUAL(false, config.is_ipv6_correct_format("0:0:0:0:0:ffff:c0a8:102"));
}

BOOST_AUTO_TEST_CASE(read_config_ipV6_no_errors) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
                    "client-work: 20\n"
                    "ip:\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0101\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0102";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_NO_THROW(config.read());
    BOOST_TEST(config.clientSleepSeconds == 10);
    BOOST_TEST(config.clientWorkSeconds == 20);
    BOOST_TEST(config.m_ipVec.size() == 2);
    BOOST_CHECK(config.m_ipVec[0] == "0000:0000:0000:0000:0000:ffff:c0a8:0101");
    BOOST_CHECK(config.m_ipVec[1] == "0000:0000:0000:0000:0000:ffff:c0a8:0102");

    after(fileName);
}

BOOST_AUTO_TEST_CASE(read_config_ipV6_ip_contain_duplicates) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
                    "client-work: 20\n"
                    "ip:\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0101\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0102\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0102";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_THROW(config.read(), std::invalid_argument);

    after(fileName);
}

BOOST_AUTO_TEST_CASE(read_config_both_v4_and_v6_addresses) {
    // given config file
    const std::string config_no_errors =
            "client-sleep: 10\n"
                    "client-work: 20\n"
                    "ip:\n"
                    "  - 0000:0000:0000:0000:0000:ffff:c0a8:0101\n"
                    "  - 192.168.10.1\n";
    before(fileName, config_no_errors);

    // when
    Config config(fileName);

    // then it should not throw exception
    BOOST_CHECK_THROW(config.read(), std::invalid_argument);

    after(fileName);
}
