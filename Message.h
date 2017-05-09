//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_MESSAGE_H
#define SAURON_MESSAGE_H

#include <utility>


enum MessageType {
  Init,
  Measurement,
  Finish,
  Ack,
  Error
};

struct Message {
    Message() {}
    Message(MessageType mtype)
        : mtype(mtype)
    {}
    MessageType mtype;
    unsigned char pipeAddress[16]; // ip, can be either ip4 or ip6
    int measureValue; // value of measurement
};


typedef std::pair<std::string, Message> MsgSenderPair;

#endif //SAURON_MESSAGE_H
