//
// Created by Przemyslaw Kopanski 13.05.17
//
#ifndef _CLIENT_TEST_OBJ_H_
#define _CLIENT_TEST_OBJ_H_

#include "Client.h"

/**
 * Wrapper for Client class to access protected fields
 */
class ClientTestObj : Client {
public:
    ClientTestObj(MessageBuffer &msgBuffer,
                  size_t port,
                  Connector *connector = nullptr,
                  ClientState state = ClientState::INIT_PHASE_FIRST)
        : Client(msgBuffer, port, connector, state)
    {}

    ClientState getState();

    void setState(const ClientState& newState);

    void runHandleStateInitPhaseFirst(const MessagePair& mpair);
};

#endif // _CLIENT_TEST_OBJ_H_
