//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_STATE_H
#define SAURON_STATE_H

enum class ClientState {
    INIT_PHASE_FIRST,
    INIT_PHASE_SECOND,
    INIT_PHASE_THIRD,
    CONNECTION_ESTABLISHED,
    FINISHING,
    FINISHED,
};

#endif //SAURON_STATE_H
