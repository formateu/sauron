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
    YAML::Node config = YAML::LoadFile(filePath);

    //TODO: add data validation
    if (config["client-sleep"]) {
        clientSleepSeconds = config["client-sleep"].as<long>();
    }

    if (config["client-work"]) {
        clientWorkSeconds = config["client-work"].as<long>();
    }
}
