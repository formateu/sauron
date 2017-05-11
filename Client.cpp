#include "Client.h"

Client::Client(MessageBuffer &msgBuffer, const std::string &address, size_t port)
    : mAddress(address)
    , msgBuffer(msgBuffer)
    , connector(Connector(msgBuffer, port))
    , state(1)
    , finish(false) {}

void Client::run() {
    std::thread listenThread(&Connector::listen, connector);

    while (!finish) {
        MessagePair messagePair = msgBuffer.pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinish(messagePair);
        }

        switch (state) {
            case 1:
                handleStateInitPhaseFirst(messagePair);
                break;
            case 2:
                handleStateInitPhaseSecond(messagePair);
                break;
            case 3:
                handleStateInitPhaseThird(messagePair);
                break;
            case 4:
                handleStateConnectionEstablished(messagePair);
                break;
            default:
                std::cout << "Undefined state" << std::endl;
        }
    }

    listenThread.join();
}

void Client::handleStateInitPhaseFirst(MessagePair messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //BLAD
        return;
    }

    predecessor = messagePair.first;
    state = 2;
    connector.send(predecessor, Message(MessageType::Ack));
}


void Client::handleStateInitPhaseSecond(MessagePair messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //BLAD
        return;
    }

    successor = messagePair.first;
    state = 3;
    connector.send(successor, Message(MessageType::Init));
}


void Client::handleStateInitPhaseThird(MessagePair messagePair) {
    if (messagePair.second.m_type != MessageType::Ack) {
        //BLAD
        return;
    }

    state = 4;
    connector.send(predecessor, Message(MessageType::Ack));
}

void Client::handleStateConnectionEstablished(MessagePair messagePair) {

}

void Client::handleFinish(MessagePair messagePair) {
    connector.send(successor, messagePair.second);
    finish = true;
}
