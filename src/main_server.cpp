//
// Created by Mateusz Forc on 03.05.17.
//

#include <iostream>

#include "Config.h"
#include "MessageBuffer.h"
#include "Connector.h"
#include "Server.h"

void showUsage(const char *prog) {
    std::cout << "Usage: " << prog << " [configfile] [port]" << std::endl;
}

std::function<void()> shutdownServer;

void signalHandler(int signum) {
    shutdownServer();
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

    Config *config = new Config(configFilePath);
    try {
        config->read();
    } catch (YAML::BadFile e) {
        std::cout << "ERROR: Cannot find config file: " << configFilePath << std::endl;
        exit(EXIT_FAILURE);
    } catch (std::invalid_argument e) {
        std::cout << "ERROR: Config file contains errors: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Config file is ok." << std::endl;

    try {
        std::shared_ptr<MessageBuffer> msgBuffer = std::make_shared<MessageBuffer>();
        Server server(msgBuffer, port, config);

        signal(SIGINT, signalHandler);
        shutdownServer = [&server](){
            server.stop();
        };

        server.run();
    } catch (std::runtime_error e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
