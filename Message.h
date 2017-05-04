#ifndef SAURON_MESSAGE_H
#define SAURON_MESSAGE_H

enum MessageType {
    Init
  , Measurement
  , Finish
  , Ack
  , Error
};

struct Message {
    MessageType mtype;
    unsigned char pipeAddress[16]; // ip, can be either ip4 or ip6
    int measureValue; // value of measurement
};

#endif //SAURON_MESSAGE_H
