//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_CLIENT_H
#define SAURON_CLIENT_H

#include <thread>
#include "Connector.h"

class Client {
public:
    Client(MsgBuffer &msgBuffer, const std::string &address, size_t port);
    void run();

private:
    Connector connector;
    const std::string &mAddress;
    MsgBuffer &msgBuffer;
    int state;
    bool finish;
    std::string predecessor;
    std::string successor;

    /**
     * Awating for predecessor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseFirst(MessagePair messagePair);

    /**
     * Awating for successor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseSecond(MessagePair messagePair);

    /**
     * Awating for ack after successor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseThird(MessagePair messagePair);

    /**
     * Connection has been established properly.
     * @param messagePair
     */
    void handleStateConnectionEstablished(MessagePair messagePair);

    /**
     * Handle error message in the first place.
     */
    void handleFinish(MessagePair messagePair);
};


#endif //SAURON_CLIENT_H
