//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_MESSAGE_H
#define SAURON_MESSAGE_H

#include <utility>
#include <string>

enum MessageType {
    Init,
    InitOk,
    InitLast,
    Run,
    Measurement,
    Finish,
    Ack,
    Terminate // mainly for testing purposes
};

struct Message {
    Message() {}

    Message(MessageType m_type) : m_type(m_type) {}

    MessageType m_type;

    unsigned char m_pipeAddress[16]; // ip, can be either ip4 or ip6

    int m_measureValue; // value of measurement
};


using MsgSenderPair = std::pair<std::string, Message>;

#endif //SAURON_MESSAGE_H
