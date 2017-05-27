//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_MESSAGE_H
#define SAURON_MESSAGE_H

#include <utility>
#include <string>
#include <cstring>

enum MessageType : char {
    Init,
    InitOk,
    InitLast,
    Run,
    Measurement,
    Finish,
    Ack,
    Terminate, // mainly for testing purposes
    OneHalfInitFinish,
};

struct Message {

    Message() {}

    Message(MessageType m_type) : m_type(m_type) {}

    Message(MessageType m_type, const char* pipeAddress) : m_type(m_type) {
        memcpy(m_pipeAddress, pipeAddress, 16);
    }

    MessageType m_type = MessageType::Init;

    unsigned char m_pipeAddress[16] = {}; // ip, can be either ip4 or ip6

    int m_measureValue = 0; // value of measurement

    unsigned m_activePeriod = 0;

    unsigned m_inactivePeriod = 0;
};

using MsgSenderPair = std::pair<std::string, Message>;

#endif //SAURON_MESSAGE_H
