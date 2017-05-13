//
// Created by Przemyslaw Kopanski
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

