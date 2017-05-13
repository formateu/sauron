//
// Created by Mateusz Forc and Wiktor Franus on 17.05.17.
//
#include "ServerState.h"

std::ostream &operator<<(std::ostream &stream, const ServerState &state) {
    const auto it = stateStringMap.find(state);
    if (it != stateStringMap.end()) {
        stream << it->second;
    } else {
        stream << "unknown serverstate";
    }

    return stream;
}
