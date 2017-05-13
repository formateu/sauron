//
// Created by Grzegorz Staniszewski on 11.05.17.
//

#ifndef SAURON_CONFIG_H
#define SAURON_CONFIG_H

#include <string>
#include <iostream>
#include <vector>

#include <yaml-cpp/yaml.h>

class Config {

    Config(const std::string &filePath);

    static const long CLIENT_SLEEP_SECONDS_DEFAULT = 10;

    static const long CLIENT_WORK_SECONDS_DEFAULT = 60;

    void read();

    //time that client should sleep in seconds
    long clientSleepSeconds;

    //time that client should work in seconds
    long clientWorkSeconds;

    std::vector<std::string> m_ipVec;

private:
    const std::string &filePath;
};

#endif //SAURON_CONFIG_H
