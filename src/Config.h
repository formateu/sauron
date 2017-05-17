//
// Created by Grzegorz Staniszewski on 11.05.17.
//

#ifndef SAURON_CONFIG_H
#define SAURON_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <yaml-cpp/yaml.h>

struct Config {

    Config(const std::string &filePath);

    static const long CLIENT_SLEEP_SECONDS_DEFAULT = 10;

    static const long CLIENT_WORK_SECONDS_DEFAULT = 60;

    void read();

    //time that client should sleep in seconds
    long clientSleepSeconds;

    //time that client should work in seconds
    long clientWorkSeconds;

    std::vector<std::string> m_ipVec;

    std::set<std::string> addresses;

private:
    bool isValidAddress(const std::string &addr);

    bool is_ipv6_address(const std::string &str);

    bool is_ipv4_address(const std::stringi &str);

    const std::string &filePath;

    YAML::Node config;
};

#endif //SAURON_CONFIG_H
