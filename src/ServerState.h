//
// Created by Mateusz Forc and Wiktor Franus on 17.05.17.
//
#ifndef SAURON_SERVERSTATE_H
#define SAURON_SERVERSTATE_H

#include <ostream>
#include <unordered_map>

enum class ServerState {
    CREATED,
    WAITING_FOR_FIRST_HALF,
    WAITING_FOR_SECOND_HALF,
    WAITING_FOR_RESULTS,
};

const std::unordered_map<ServerState, std::string> stateStringMap = {
    { ServerState::CREATED,                     "Created" },
    { ServerState::WAITING_FOR_FIRST_HALF,      "Waiting for first half" },
    { ServerState::WAITING_FOR_SECOND_HALF,     "Waiting for second half" },
    { ServerState::WAITING_FOR_RESULTS,         "Waiting for results" },
};

std::ostream &operator<<(std::ostream &stream, const ServerState &state);

#endif //SAURON_SERVERSTATE_H
