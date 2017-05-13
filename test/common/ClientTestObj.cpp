//
// Created by Przemyslaw Kopanski on 13.05.17
//
#include "ClientTestObj.h"

ClientState ClientTestObj::getState() {
    return state;
}

void ClientTestObj::setState(const ClientState& newState) {
    state = newState;
}

void ClientTestObj::runHandleStateInitPhaseFirst(const MessagePair& mpair) {
    handleStateInitPhaseFirst(mpair);
}

