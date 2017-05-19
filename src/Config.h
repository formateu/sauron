//
// Created by Grzegorz Staniszewski on 11.05.17.
//

#ifndef SAURON_CONFIG_H
#define SAURON_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>

#include <arpa/inet.h>
#include <yaml-cpp/yaml.h>


struct ConfigBase {
    ConfigBase() {}

    ConfigBase(long, long);

    static const long CLIENT_SLEEP_SECONDS_DEFAULT = 10;

    static const long CLIENT_WORK_SECONDS_DEFAULT = 60;

    //time that client should sleep in seconds
    long clientSleepSeconds;

    //time that client should work in seconds
    long clientWorkSeconds;

    std::vector<std::string> m_ipVec;

    bool is_ipv6_correct_format(const std::string &addr);
};

struct Config : public ConfigBase {
    Config(const std::string &filePath);

    void read();

    std::set<std::string> addresses;
private:

    void split_addresses_by_ip_version(std::set<std::string> &v4_set, std::set<std::string> &v6_set);

    bool is_ipv4_address(const std::string &str);

    bool is_ipv6_address(const std::string &str);

    const std::string &filePath;

    YAML::Node config;
};

struct MockConfig : public ConfigBase { };

#endif //SAURON_CONFIG_H
