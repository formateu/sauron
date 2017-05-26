//
// Created by Mateusz Forc and Wiktor Franus 19.05.17
//
#ifndef _SERVER_TEST_OBJ_H_
#define _SERVER_TEST_OBJ_H_

#include "Server.h"

/**
 * Wrapper for Server class to access protected fields
 */
class ServerTestObj : public Server {
public:
    ServerTestObj(MessageBuffer &msgBuffer,
                  const std::string &address,
                  size_t port,
                  ConfigBase *conf,
                  Connector *connector = nullptr,
                  ServerState state = ServerState::CREATED)
            : Server(msgBuffer, port, conf, connector, state)
    {}

    ServerState getState();

    void setState(const ServerState& newState);
};

#endif // _SERVER_TEST_OBJ_H_
