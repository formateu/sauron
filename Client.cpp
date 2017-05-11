#include "Client.h"

Client::Client(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               ClientState state)
    : mAddress(address)
    , msgBuffer(msgBuffer)
    , connector(Connector(msgBuffer, port))
    , state(state) {}

void Client::run() {
    std::thread listenThread(&Connector::listen, connector);

    while (state != ClientState::FINISHED) {
        MessagePair messagePair = msgBuffer.pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinishing(messagePair);
        }
        try {
            router[state](messagePair);
        } catch (const std::out_of_range &e) {
            std::cout << "[ERROR]: Unkown state. " << e.what() << std::endl;
        }
    }

    listenThread.join();
}

void Client::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //BLAD
        return;
    }

    predecessor = messagePair.first;
    state = ClientState::INIT_PHASE_SECOND;
    connector.send(predecessor, Message(MessageType::Ack));
}


void Client::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //BLAD
        return;
    }

    successor = messagePair.first;
    state = ClientState::INIT_PHASE_THIRD;
    connector.send(successor, Message(MessageType::Init));
}


void Client::handleStateInitPhaseThird(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Ack) {
        //BLAD
        return;
    }

    state = ClientState::CONNECTION_ESTABLISHED;
    connector.send(predecessor, Message(MessageType::Ack));
}

void Client::handleStateConnectionEstablished(const MessagePair &messagePair) {

}

void Client::handleFinishing(const MessagePair &messagePair) {
    connector.send(successor, messagePair.second);
    state = ClientState::FINISHED;
}
