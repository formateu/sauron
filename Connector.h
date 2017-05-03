//
// Created by Mateusz Forc and Wiktor Franus on 03.05.17.
//

#ifndef SAURON_CONNECTOR_H
#define SAURON_CONNECTOR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>

/**
 * Simple BSD c++ wrapper
 */

class Message;

class Connector {
public:
    Connector() {}
    Connector(size_t listenPort);

    void send(const std::string &address, size_t port, const Message&msg);
    void listen(std::vector<Message> &msgBuffer);
    void terminate();
protected:
    size_t m_listenPort;
    int m_listenSocket;
};

#endif //SAURON_CONNECTOR_H
