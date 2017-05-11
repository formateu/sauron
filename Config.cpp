//
// Created by Grzegorz Staniszewski on 11.05.17.
//

#include "Config.h"

Config::Config(const std::string& filePath)
    : filePath(filePath) {
}

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