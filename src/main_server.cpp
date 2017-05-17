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
    std::cout << "Usage: "
              << prog
              << " [-r configfile] | [-t unit|int] | -h"
              << std::endl;
}

int main(int argc, char **argv) {
    int opt;
    std::string stringParam;
    std::string configFilePath;
    opt = getopt(argc, argv, "r:t:h");

    switch (opt) {
        case 'r':
            configFilePath = optarg;
            break;
        case 't':
            stringParam = optarg;
            break;
        default:
            showUsage(argv[0]);
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
    }

    MessageBuffer msgBuffer;
    Server server(msgBuffer, "127.0.0.1", 3000, config);
    signal(SIGABRT, sigabrtHandler);
    server.run();

    std::cout << config.clientSleepSeconds << std::endl;
    std::cout << config.clientWorkSeconds << std::endl;

    return 0;
}
