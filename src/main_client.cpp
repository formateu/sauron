//
// Created by Grzegorz Staniszewski on 11.05.17.
//
#include <iostream>
#include <csignal>

#include "MessageBuffer.h"
#include "Client.h"

void showUsage(const char *prog) {
    std::cout << "Usage: " << prog << " [port]" << std::endl;
}

std::function<void()> shutdownServer;

void signalHandler(int signum) {
    shutdownServer();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    std::string portStr = argv[1];
    int port;
    try {
        port = std::stoi(portStr);
    } catch (std::invalid_argument e) {
        std::cout << "ERROR: Cannot read port: " << portStr << std::endl;
        exit(EXIT_FAILURE);
    }

    std::shared_ptr<SplitMessageBuffer> msgBuffer = std::make_shared<SplitMessageBuffer>();
    Client client(msgBuffer, port);
    signal(SIGINT, signalHandler);
        shutdownServer = [&client](){
        client.stop();
    };

    try {
        client.run();
    } catch (std::runtime_error e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}
