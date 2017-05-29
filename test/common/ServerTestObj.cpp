//
// Created by Mateusz Forc and Wiktor Franus 19.05.17
//
#include "ServerTestObj.h"

ServerState ServerTestObj::getState() {
    return m_state;
}

void ServerTestObj::setState(const ServerState& newState) {
    m_state = newState;
}

void ServerTestObj::setConnector(Connector *connector) {
    connector = connector;
}