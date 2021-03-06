//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_STATE_H
#define SAURON_STATE_H

#include <ostream>
#include <unordered_map>

enum class ClientState {
    INIT_PHASE_FIRST,
    INIT_PHASE_SECOND,
    INIT_PHASE_LAST_SECOND,
    CONNECTION_ESTABLISHED,
    MEASURE_TIME,
    FINISH,
};

const std::unordered_map<ClientState, std::string> stateStringMap = {
    { ClientState::INIT_PHASE_FIRST,       "Init phase first" },
    { ClientState::INIT_PHASE_SECOND,      "Init phase second" },
    { ClientState::CONNECTION_ESTABLISHED, "Connection established" },
    { ClientState::MEASURE_TIME,           "Connection established" },
    { ClientState::FINISH,                 "Finish" }
};

std::ostream &operator<<(std::ostream &stream, const ClientState &state);

#endif //SAURON_STATE_H
