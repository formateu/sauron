//
// Created by Wiktor Franus and Mateusz Forc on 04.05.17.
//

#ifndef SAURON_MESSAGEBUFFER_H
#define SAURON_MESSAGEBUFFER_H


#include <queue>
#include <mutex>
#include "Message.h"

class MessageBuffer {
public:
    using MsgSenderPair = std::pair<Message, std::string>;
    MessageBuffer() {}
    ~MessageBuffer() {}

    void push(const MsgSenderPair& msg);
    MsgSenderPair pop();
protected:
    auto m_cmp = [](const MsgSenderPair& left, const MsgSenderPair& right) {
         return left.first.mtype == MessageType::Error;
    };
    std::priority_queue<MsgSenderPair, std::vector<MsgSenderPair>, decltype(m_cmp)> m_queue(m_cmp);
    std::mutex m_mutex;
};


#endif //SAURON_MESSAGEBUFFER_H
