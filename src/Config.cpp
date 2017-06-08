//
// Created by Grzegorz Staniszewski on 11.05.17.
//
#include <netinet/in.h>
#include "Config.h"

ConfigBase::ConfigBase(long sleepTime, long workTime)
    : clientSleepSeconds(sleepTime)
    , clientWorkSeconds(workTime)
{}

Config::Config(const std::string& filePath)
    : ConfigBase(CLIENT_SLEEP_SECONDS_DEFAULT, CLIENT_WORK_SECONDS_DEFAULT)
    , filePath(filePath)
{}

void Config::read() {
    config = YAML::LoadFile(filePath);

    if (config["client-sleep"]) {
        try {
            clientSleepSeconds = config["client-sleep"].as<unsigned>();
        } catch (YAML::BadConversion e) {
            throw std::invalid_argument("Invalid parameter client-sleep.");
        }
    }

    if (config["client-work"]) {
        try {
            clientWorkSeconds = config["client-work"].as<unsigned>();
        } catch (YAML::BadConversion e) {
            throw std::invalid_argument("Invalid parameter client-work.");
        }
    }

    if (config["ip"]) {
        try {
            m_ipVec = config["ip"].as<std::vector<std::string>>();
        } catch (YAML::BadConversion e) {
            throw std::invalid_argument("Ip list empty or contains errors.");
        }

        std::set<std::string> v4_set, v6_set;
        split_addresses_by_ip_version(v4_set, v6_set);

        //check if addresses are in single ip version
        if (v4_set.size() != 0 && v6_set.size() != 0) {
            throw std::invalid_argument("IP list should contain either v4 or v6 ip addresses.");
        }

        //check for duplicates
        if (v4_set.size() != m_ipVec.size() && v6_set.size() != m_ipVec.size()) {
            throw std::invalid_argument("IP list contains duplicates.");
        }

    } else {
        throw std::invalid_argument("IP list not provided.");
    }
}

void Config::split_addresses_by_ip_version(std::set<std::string> &v4_set, std::set<std::string> &v6_set) {
    std::for_each(m_ipVec.begin(), m_ipVec.end(),
        [&](auto &it) {
            if (this->is_ipv4_address(it)) {
                v4_set.insert(it);
            } else if (this->is_ipv6_address(it)) {

                if (!this->is_ipv6_correct_format(it)) {
                    throw std::invalid_argument("IPv6 address cannot be passed in short : " + it + ".");
                }

                v6_set.insert(it);
            } else {
                throw std::invalid_argument("Invalid IP address: " + it + ".");
            }
    });
}

bool Config::is_ipv4_address(const std::string &addr) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, addr.c_str(), &(sa.sin_addr)) == 1;
}

bool Config::is_ipv6_address(const std::string &addr) {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, addr.c_str(), &(sa.sin6_addr)) == 1;
}

bool ConfigBase::is_ipv6_correct_format(const std::string &addr) {
    std::regex format("((.{4}):){7}(.{4})");
    return std::regex_search(addr, format);
}
