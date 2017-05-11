//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#include "ClientState.h"

std::ostream& operator<<(std::ostream& stream, const ClientState& state) {
    const auto it = stateStringMap.find(state);
    if (it != stateStringMap.end())
      stream << it->second;
    else
      stream << "unknown clientstate";
    return stream;
}
