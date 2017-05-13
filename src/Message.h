//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_MESSAGE_H
#define SAURON_MESSAGE_H

#include <utility>
#include <string>
#include <cstring>

enum MessageType {
    Init,
    Measurement,
    Finish,
    Ack,
    Error,
    Terminate, // mainly for testing purposes
    OneHalfInitFinish,
    InitOk,
    Last,
    Run
};

struct Message {

    Message() {}

    Message(MessageType m_type) : m_type(m_type) {}

    Message(MessageType m_type, const char* pipeAddress) : m_type(m_type) {
        //for(int i=0; i<16; )
        memcpy(m_pipeAddress, pipeAddress, 16);
    }

    MessageType m_type;

    unsigned char m_pipeAddress[16]; // ip, can be either ip4 or ip6

    int m_measureValue; // value of measurement
};

using MsgSenderPair = std::pair<std::string, Message>;

#endif //SAURON_MESSAGE_H
