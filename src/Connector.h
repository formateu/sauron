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

    virtual void shutdownListenThread() = 0;

    virtual ~Connector() {}
};


class InternetConnector : public Connector {
public:
    using MsgBufSharedPtr = std::shared_ptr<MessageBufferBase>;
    InternetConnector(MsgBufSharedPtr buffer, size_t listenPort);

    ~InternetConnector();

    void send(const std::string &address, const Message &msg);

    void listen();

    void terminate() {}

    void shutdownListenThread();

private:
    size_t m_port;

    int m_listenSocket;

    MsgBufSharedPtr m_msgBuffer;

    bool m_shutdownPerformed = false;

    void closeSocket();
};


class MockConnector : public Connector {
public:
    using MsgBufSharedPtr = std::shared_ptr<MessageBufferBase>;

    MockConnector(MsgBufSharedPtr buffer);

    void send(const std::string &address, const Message &msg);

    void listen();

    void shutdownListenThread() {}

private:
    std::shared_ptr<MessageBufferBase> m_msgBuffer;
};


#endif //SAURON_CONNECTOR_H
