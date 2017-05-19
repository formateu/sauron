//
// Created by Mateusz Forc and Wiktor Franus on 03.05.17.
//
#ifndef SAURON_CONNECTOR_H
#define SAURON_CONNECTOR_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <memory>
#include <vector>

#include "Message.h"
#include "MessageBuffer.h"

/**
 * Simple BSD c++ wrapper
 */

typedef std::pair<std::string, Message> MessagePair;

class Connector {
public:
    virtual void send(const std::string &address, const Message &msg) = 0;

    virtual void listen() = 0;

    virtual ~Connector() {}
};


class InternetConnector : public Connector {
public:
    InternetConnector(MessageBuffer &buffer, size_t listenPort);

    ~InternetConnector();

    void send(const std::string &address, const Message &msg);

    void listen();

    void terminate() {}

private:
    size_t m_port;

    int m_listenSocket;

    MessageBuffer &msgBuffer;

    void closeSocket();
};


class MockConnector : public Connector {
public:
    MockConnector(MessageBuffer &buffer);

    void send(const std::string &address, const Message &msg);

    void listen();

private:
    MessageBuffer &msgBuffer;
};


#endif //SAURON_CONNECTOR_H
