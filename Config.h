//
// Created by Grzegorz Staniszewski on 11.05.17.
//

#ifndef SAURON_CONFIG_H
#define SAURON_CONFIG_H

#include <string>
#include <iostream>

#include <yaml-cpp/yaml.h>

struct Config {
    Config(const std::string& filePath);

    void read();

    //time that client should sleep in seconds
    long clientSleepSeconds = 10;

    //time that client should work in seconds
    long clientWorkSeconds = 60;

private:
    const std::string &filePath;
};

#endif //SAURON_CONFIG_H
