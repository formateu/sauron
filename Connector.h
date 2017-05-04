//
// Created by Mateusz Forc and Wiktor Franus on 03.05.17.
//

#ifndef SAURON_CONNECTOR_H
#define SAURON_CONNECTOR_H

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

class MsgBuffer {
public:
    void push() {}

    MessagePair pop() {
        Message message;
        return {"192.168.1.2", message};
    }
};

class Connector {
public:
    Connector(MsgBuffer &msgBuffer, size_t listenPort);

    void send(const std::string &address, const Message&msg);
    void listen();
    void terminate() {}

protected:
    size_t m_port;
    int m_listenSocket;
    MsgBuffer &msgBuffer;
};

#endif //SAURON_CONNECTOR_H
