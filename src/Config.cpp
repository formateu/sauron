//
// Created by Grzegorz Staniszewski on 11.05.17.
//
#include "Config.h"

Config::Config(const std::string& filePath)
    : filePath(filePath)
    , clientSleepSeconds(CLIENT_SLEEP_SECONDS_DEFAULT)
    , clientWorkSeconds(CLIENT_WORK_SECONDS_DEFAULT)
{}

void Config::read() {
    config = YAML::LoadFile(filePath);

    if (config["client-sleep"]) {
        try {
            clientSleepSeconds = config["client-sleep"].as<long>();
        } catch (YAML::BadConversion e) {
            throw std::invalid_argument("Invalid parameter client-sleep.");
        }
    }

    if (config["client-work"]) {
        try {
            clientWorkSeconds = config["client-work"].as<long>();
        } catch (YAML::BadConversion e) {
            throw std::invalid_argument("Invalid parameter client-work.");
        }
    }

    std::vector<std::string> addrConfig;

    if (config["ip"]) {
        addrConfig = config["ip"].as<std::vector<std::string>>();

        std::for_each(addrConfig.begin(), addrConfig.end(),
            [this](auto &it){
                if (this->isValidAddress(it)) {
                    this->addresses.insert(it);
                } else {
                    throw std::invalid_argument("Invalid IP address: " + it + ".");
                }
        });
    }

    if (addresses.size() == 0){
        throw std::invalid_argument("IP list is empty.");
    } else if (addresses.size() != addrConfig.size()) {
        throw std::invalid_argument("IP list contains duplicates.");
    }

    m_ipVec = std::move(addrConfig);
}

bool Config::isValidAddress(const std::string &addr) {
    return is_ipv4_address(addr) || is_ipv6_address(addr);
}

bool Config::is_ipv4_address(const std::string &str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) == 1;
}

bool Config::is_ipv6_address(const std::string& str) {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)) == 1;
}
