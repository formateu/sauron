//
// Created by Przemyslaw Kopanski on 13.05.17
//
#include "ClientTestObj.h"

ClientState ClientTestObj::getState() {
    return m_state;
}

void ClientTestObj::setState(const ClientState& newState) {
    m_state = newState;
}

void ClientTestObj::runHandleStateInitPhaseFirst(const MessagePair& mpair) {
    handleStateInitPhaseFirst(mpair);
}

