//
// Created by Mateusz Forc on 03.05.17.
//

#include <iostream>
#include <csignal>
#include <unistd.h>

#include "MessageBuffer.h"
#include "Connector.h"
#include "Server.h"
#include "Config.h"

extern char *optarg;

void sigabrtHandler(int flet) {
    exit(0);
}

void showUsage(const char *prog) {
    std::cout << "Usage: " << prog << " [configfile] [port]" << std::endl;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    std::string configFilePath = argv[1];
    std::string portStr = argv[2];
    int port;
    try {
        port = std::stoi(portStr);
    } catch (std::invalid_argument e) {
        std::cout << "ERROR: Cannot read port: " << portStr << std::endl;
        exit(EXIT_FAILURE);
    }

    Config config(configFilePath);
    try {
        config.read();
    } catch (YAML::BadFile e) {
        std::cout << "ERROR: Cannot find config file: " << configFilePath << std::endl;
        exit(EXIT_FAILURE);
    } catch (std::invalid_argument e) {
        std::cout << "ERROR: Config file contains errors: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Config file is ok." << std::endl;

    try {
        MessageBuffer msgBuffer;
        Server server(msgBuffer, port, &config);
        signal(SIGABRT, sigabrtHandler);
        server.run();
    } catch (std::runtime_error e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
